/**
 *
 * @file      hmi/cli.cpp
 *
 * @brief     This is the implementation of the Customisation CLI
 *
 *            This file is part of the Customisation framework (see link).
 *
 * @author    Olivier Stoltz-Douchet <ezdayo@gmail.com>
 *
 * @copyright (c) 2019-2020 Olivier Stoltz-Douchet
 * @license   http://opensource.org/licenses/MIT MIT
 * @link      https://github.com/ezdayo/customisation
 *
 **/

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <set>
#include <thread>

#include "customisation/cli.hpp"
#include "customisation/io.hpp"
#include "log.hpp"

extern "C" {
#include <readline/history.h>
#include <readline/readline.h>
}

namespace Customisation {
 
/* Local static pointer to the CLI */
static CLI *cli = nullptr;

/* CLI mode management */
static bool is_interactive  = false;

/* Sorted sets of entities, parameters, commands */
static const std::set<std::string> none {};
static const auto &                files = none;
static const auto &                whitelist = none;
static const std::set<std::string> getables { "description", "traits",
                                              "type", "value", "values" };
static const std::set<std::string> showables { "configurables", "configured",
                                               "entities", "getters",
                                               "parameters", "setters",
                                               "snapshot", "structure" };

static std::set<std::string>       components;
static std::set<std::string>       entities;
static std::set<std::string>       parameters;
static std::set<std::string>       cmds;

static std::map<std::string, const std::set<std::string> *> targets4cmd;
static std::map<std::string, const std::set<std::string> *> values4cmd;

static std::map<std::string, char* (*)(const char*, int)> check_targets4cmd;
static std::map<std::string, char* (*)(const char*, int)> check_values4cmd;

static std::string dequote(std::string &input) noexcept {
    if ( (input.front() == '"') || (input.front() == '\'') ) {
        ASSERT(( (input.back() == '"') || (input.back() == '\'') ),
                "B2bot::DScribe::dequote(): unbalanced quoted string "
                "provided:\n    %s", input.c_str());
        return (input.substr(1, input.length()-2));
    }
    return input;
}

static std::set<std::string> get_whitelist(const std::string &input) noexcept {
    std::set<std::string> wl;
    std::list<std::string> tokens;
    Customisation::IO::parse(input, tokens);

    bool found_wl = false;
    for (auto &w : tokens) {
        if (found_wl) {
            if (w != "}") {
                if (w != ",") {
                    wl.emplace(std::move(dequote(w)));
                }
            } else {
                found_wl = false;
            }
        } else {
            if (w == "{") {
                found_wl = true;
            } else if (w == "not") {
                return wl;
            }
        } 
    }

    return wl;
}

static char *check(const char *text, int state,
                   const std::set<std::string> &valid) {
    static int len;
    static std::set<std::string>::iterator it;

    /* All static data is to be reset when state is null (new search) */
    if (state == 0) {
        len = strlen(text);
        it = valid.begin();
    }

    while (it != valid.end()) {
        const auto name = it->c_str();
        ++it;
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return nullptr;
}

static char *check_none(const char* /*text*/, int /*state*/)
{
    return nullptr;
}

static char *check_files(const char* /*text*/, int /*state*/)
{
    /* Complete against file names */
    rl_attempted_completion_over = 0;
    return nullptr;
}

static char *check_components(const char *text, int state)
{
    return check(text, state, components);
}

static char *check_entities(const char *text, int state)
{
    return check(text, state, entities);
}

static char *check_parameters(const char *text, int state)
{
    return check(text, state, parameters);
}

static char *check_cmds(const char *text, int state)
{
    return check(text, state, cmds);
}

static char *check_getables(const char *text, int state)
{
    return check(text, state, getables);
}

static char *check_showables(const char *text, int state)
{
    return check(text, state, showables);
}

static char *check_whitelist(const char *text, int state)
{
    static std::set<std::string> valids;

    if (state == 0) {
        valids.clear();

        /* Get the target string */
        std::string line(rl_line_buffer);
        std::list<std::string> tokens;
        Customisation::IO::parse(line, tokens, 3);
        tokens.pop_front();
        auto target = tokens.front();
    
        /* Get the target values */    
        bool ok;
        auto &root = cli->get_component(target, ok);
        if (! ok) {
            return nullptr;
        }
        valids = get_whitelist(root.values());
    }

    return check(text, state, valids);
}

/* The completion function: most commands have the form
 * COMMAND TARGET VALUE */
static char **complete(const char *text, int start, int /*end*/) {
    /* Do not complete against file names by default */
    rl_attempted_completion_over = 1;

    /* But complete quoted stuff */
    rl_completer_quote_characters = "\"'";

    /* This is definitely a command completion */
    if (start == 0) {
        return rl_completion_matches(text, check_cmds);
    }

    /* This is no longer the command so let's parse the whole line buffer */
    std::string line(rl_line_buffer);
    std::list<std::string> tokens;
    Customisation::IO::parse(line, tokens, 3);
    /* Do not check for parse error since there can be some when completing
     * some unfinished item */
    if (tokens.size() == 0) {
        return nullptr;
    }

    /* Get the current command */
    auto cmd = tokens.front();
    tokens.pop_front();

    /* If this is an unknown command, then it cannot be completed */
    auto tgts_check = check_targets4cmd.find(cmd);
    if (tgts_check == check_targets4cmd.end()) {
        return nullptr;
    }

    /* If there is no more than one token, then we search for the target */
    if (((tokens.size() == 1) && (tokens.front() == text)) ||
         (tokens.size() == 0) ) {
        return rl_completion_matches(text, *(tgts_check->second));
    }

    /* Otherwise we are looking for the value item */
    auto tgt = tokens.front();
    tokens.pop_front();

    /* If this is an unknown target, or if there is more than one more item on
     * the current line, then the line cannot be completed */
    auto tgts = *targets4cmd[cmd];
    if ((tgts.find(tgt) == tgts.end()) || (tokens.size() >= 2) ) {
        return nullptr;
    }

    /* Complete the last bit */
    auto conts_check = check_values4cmd.find(cmd);
    return rl_completion_matches(text, *(conts_check->second));
}

/* A simple macro to map a name to a lambda-ised cmd_func (to capture this) */
#define COMMAND(name, func, tgt, value) \
    command.emplace(#name, [this] (std::list<std::string> &tokens) { \
                                return this->cmd_##func(tokens); }); \
    cmds.emplace(#name); \
    targets4cmd.emplace(#name, &tgt); \
    check_targets4cmd.emplace(#name, check_##tgt); \
    values4cmd.emplace(#name, &value); \
    check_values4cmd.emplace(#name, check_##value)

CLI::CLI(Customisation::Entity &c) noexcept : core(c), command() {
    
    /* Store the CLI so that readline can introspect into the core */
    cli = this;

    /* Setup entities and parameters, and clear completion map */
    components.clear();
    entities = { "*" };
    for (auto &e : core.entities()) {
        entities.insert(e);
        components.emplace(e);
    }

    parameters.clear();
    for (auto &p : core.parameters()) {
        parameters.insert(p);
        components.emplace(p);
    }
    cmds.clear();

    targets4cmd.clear();
    values4cmd.clear();
    
    check_targets4cmd.clear();
    check_values4cmd.clear();

    /* Define all supported commands */
    COMMAND(exit, exit, none, none);
    COMMAND(quit, exit, none, none);
    COMMAND(finalise, finalise, entities, none);
    COMMAND(get, get, components, getables);
    COMMAND(initialise, initialise, entities, none);
    COMMAND(load, load, entities, files);
    COMMAND(prompt, prompt, none, none);
    COMMAND(run, run, files, none);
    COMMAND(save, save, entities, files);
    COMMAND(show, show, entities, showables);
    COMMAND(set, set, parameters, whitelist);
    COMMAND(wait, wait, none, none);

    /* Use the specific completion function */
    rl_attempted_completion_function = complete;
}

static bool check_and_extract(std::list<std::string> &tokens,
                              std::string &cmd, std::string &target, 
                              std::string &value, const char *mode,
                              const char *valids) noexcept {
    /* Check that there is at least one token! */
    ASSERT( (tokens.size() != 0), "No command provided!");

    /* Get'n check the command */
    cmd = tokens.front();
    tokens.pop_front();
    
    // if the command exists then it has a targets4cmd entry, so keep it for
    // the next check
    auto targets = targets4cmd.find(cmd);
    if (targets == targets4cmd.end()) {
        LOGE("Unknown command '%s'!", cmd.c_str());
        return false;
    }
    
    auto tgts_check = check_targets4cmd.find(cmd);
    /* Get'n check the target (if any) */
    if (tgts_check->second == check_none) {
        if (tokens.size() != 0) {
            LOGE("Invalid number of arguments!\n  > Usage: %s", cmd.c_str());
            return false;
        }

        return true;
    }
 
    if (strlen(mode) == 0) {
        if (tokens.size() != 1) {
            LOGE("Invalid number of arguments!\n  > Usage: %s target", 
                  cmd.c_str());
            return false;
        }
    } else {
        if (tokens.size() != 2) {
            LOGE("Invalid number of arguments!\n  > Usage: %s target %s", 
                  cmd.c_str(), valids);
            return false;
        }
    }
    target = tokens.front();
    tokens.pop_front();

    if ( (tgts_check->second != check_files) &&
         (targets->second->find(target) == targets->second->end()) ) {
        LOGE("Unknown target '%s'!", target.c_str());
        return false;
    }

    if (tokens.size() == 0) {
        return true;
    }

    /* Get'n check the value (if any) */
    auto values = values4cmd.find(cmd);
    value = tokens.front();

    if ( (values->second->size() != 0) &&
         (values->second->find(value) == values->second->end()) ) {
        LOGE("Unknown %s '%s'!", mode, value.c_str());
        return false;
    }

    return true;
}

static bool check_and_extract(std::list<std::string> &tokens,
                              std::string &cmd, std::string &target, 
                              std::string &value, const char *mode) noexcept {
    return check_and_extract(tokens, cmd, target, value, mode, mode);
}

static bool check_and_extract(std::list<std::string> &tokens,
                              std::string &cmd, std::string &target, 
                              std::string &value) noexcept {
    return check_and_extract(tokens, cmd, target, value, "");
}

bool CLI::cmd_exit(std::list<std::string> &tokens) noexcept {
    std::string cmd, target, value;
    if (check_and_extract(tokens, cmd, target, value)) {
        /* Stopping the CLI */
        return false;
    }
    return true;
}

bool CLI::cmd_finalise(std::list<std::string> &tokens) noexcept {
    std::string cmd, target, value;
    if (check_and_extract(tokens, cmd, target, value)) {
        bool ok;
        auto &root = get_entity(target, ok);
        if (!ok) {
            return true;
        }

        root.finalise();
    }
    return true;
}

bool CLI::cmd_get(std::list<std::string> &tokens) noexcept {
    std::string cmd, target, item;
    if (check_and_extract(tokens, cmd, target, item, "mode",
                          "description|traits|type|value|values")) {
        bool ok;
        auto &root = get_component(target, ok);
        if (!ok) {
            return true;
        }
       
        std::cout << target << " = "; 
        if (item == "description") {
            std::cout << root.description();
        } else if (item == "traits") {
            std::cout << Customisation::toString(root.traits());
        } else if (item == "type") {
            std::cout << root.type();
        } else if (item == "value") {
            std::cout << root.value();
        } else if (item == "values") {
            std::cout << root.values();
        } else {
            LOGE("Cannot get item '%s' for '%s'!",
                 item.c_str(), target.c_str());
        }
        std::cout << std::endl;
    }
    return true;
}

bool CLI::cmd_initialise(std::list<std::string> &tokens) noexcept {
    std::string cmd, target, value;
    if (check_and_extract(tokens, cmd, target, value)) {
        bool ok;
        auto &root = get_entity(target, ok);
        if (!ok) {
            return true;
        }

        auto error = root.initialise();
        if (error != Customisation::Error::NONE) {
            LOGE("Cannot initialise '%s': error %d", 
                 target.c_str(), static_cast<int>(error));
        }
    }
    return true;
}

bool CLI::cmd_load(std::list<std::string> &tokens) noexcept {
    std::string cmd, target, file;
    if (check_and_extract(tokens, cmd, target, file, "file", "<file>")) {
        bool ok;
        auto &root = get_entity(target, ok);
        if (!ok) {
            return true;
        }
   
        if (core.locked()) {
            LOGE("Cannot load configuration on an initialised %s!", 
                 core.name().c_str());
            return true;
        }

        Customisation::Error error = Customisation::Error::NONE;
        auto cfg = Customisation::IO::loaded(file, error);
    
        if (error == Customisation::Error::NONE) {

            int line = 0;
            error = root.configure(cfg, line);

            if (error != Customisation::Error::NONE) {
                LOGE("Loading '%s': error %d at line %d",
                    file.c_str(), static_cast<int>(error), line);
            }
        } else {
                LOGE("Cannot load configuation '%s' for %s: error %d",
                    file.c_str(), target.c_str(), static_cast<int>(error));
        }
    }

    return true;
}

bool CLI::cmd_prompt(std::list<std::string> &/*tokens*/) noexcept {
    is_interactive = true;
    return true;
}

bool CLI::cmd_run(std::list<std::string> &tokens) noexcept {
    std::string cmd, name, dummy;
    if (check_and_extract(tokens, cmd, name, dummy)) {
        std::ifstream file(name, std::ios_base::in);

        if (!file.is_open()) {
            LOGE("Unable to find '%s' script to run", name.c_str());
            return true;
        }

        for (std::string line; std::getline(file, line); /**/) {
            if (line.size() > 0) {
                add_history(line.c_str());
            }

            if (! parse(line)) {
                return false;
            }
        }
    }
    return true;
}

bool CLI::cmd_save(std::list<std::string> &tokens) noexcept {
    std::string cmd, target, file;
    if (check_and_extract(tokens, cmd, target, file, "file", "<file>")) {
        bool ok;
        auto &root = get_entity(target, ok);
        if (!ok) {
            return true;
        }

        auto error = Customisation::IO::save(file, root.configured());
        if (error != Customisation::Error::NONE) {
            LOGE("Unable to save '%s': error %d",
                 file.c_str(), static_cast<int>(error));
        }
    }

    return true;
}

bool CLI::cmd_show(std::list<std::string> &tokens) noexcept {
    std::string cmd, target, item;
    if (check_and_extract(tokens, cmd, target, item, "type",
                          "configurables|configured|entities|getters|"
                          "parameters|setters|snapshot|structure")) {
        bool ok;
        auto &root = get_entity(target, ok);
        if (!ok) {
            return true;
        }

        std::vector<std::string> status;
        if (item == "configurables") {
            status = root.configurables();
        } else if (item == "configured") {
            status = root.configured();
        } else if (item == "entities") {
            status = root.entities();
        } else if (item == "getters") {
            status = root.getters();
        } else if (item == "parameters") {
            status = root.parameters();
        } else if (item == "setters") {
            status = root.setters();
        } else if (item == "snapshot") {
            status = root.snapshot();
        } else if (item == "structure") {
            status = root.structure();
        } else {
            LOGE("Cannot show item '%s' for '%s'!",
                 item.c_str(), target.c_str());
        }
    
        for (auto &l : status) {
            std::cout << l << std::endl;
        }
    }

    return true;
}

bool CLI::cmd_set(std::list<std::string> &tokens) noexcept {
    std::string cmd, target, value;
    if (check_and_extract(tokens, cmd, target, value, "value")) {
        bool ok;
        auto &root = get_parameter(target, ok);
        if (!ok) {
            return true;
        }

        Customisation::Error error;
        if (root.locked()) {
            error = root.set(value);
        } else {
            error = root.configure(value);
        }

        if (error != Customisation::Error::NONE) {
            LOGE("Unable to set %s to %s: error %d", 
                 target.c_str(), value.c_str(), static_cast<int>(error));
        } else {
            std::list<std::string> get_back({ "get", target, "value" });
            return cmd_get(get_back);
        }
    }
    return true;
}

static void dummy_line_handler(char* /*line*/) {
}

bool CLI::cmd_wait(std::list<std::string> &/*tokens*/) noexcept {
    rl_callback_handler_install("", dummy_line_handler);
    rl_callback_read_char();
    rl_replace_line("", 0);
    rl_redisplay();
    rl_callback_handler_remove();
    
    return true;
}


bool CLI::parse(std::string &data) noexcept {
    std::list<std::string> tokens;
    auto error = Customisation::IO::parse(data, tokens, 3);

    if (error != Customisation::Error::NONE) {
        LOGE("Parse error %d for:\n %s",
             static_cast<int>(error), data.c_str());
        return true;
    }

    if (!tokens.empty()) {
        auto it = command.find(tokens.front());
        if (it != command.end()) {
            auto again = it->second(tokens);
            if (! again) {
                return false;
            }
        } else {
            LOGE("Invalid request '%s'", data.c_str());
        }
    }

    return true;
}

void CLI::interactive() noexcept {
    char *buf;
    std::string prompt(core.name() +"> ");  
    is_interactive = true;
    while((buf = ::readline(prompt.c_str())) != nullptr) {
        std::string data(buf);
        if (strlen(buf) > 0) {
            add_history(buf);
        }
        free(buf);

        if (! parse(data)) {
            break;
        }
    }
}

void CLI::script(const std::string &name) noexcept {
    std::list<std::string> cmd({"run", name });
    cmd_run(cmd);

    if (is_interactive) {
        interactive();
    }
}

Customisation::Component &CLI::get_component(const std::string &target,
                                             bool &ok) noexcept {
    ok = true;
    auto &c = core.component(target);
    if (&c == &Customisation::Component::INVALID) {
        LOGE("Invalid component provided '%s'", target.c_str());
        ok = false;
    }

    return c;
}

Customisation::Entity &CLI::get_entity(const std::string &target, 
                                       bool &ok) noexcept {
    ok = true;
    auto &e = core.component(target).entity();
    if (&e == &Customisation::Entity::INVALID) {
        LOGE("Invalid entity provided '%s'", target.c_str());
        ok = false;
    }

    return e;
}

Customisation::Component &CLI::get_parameter(const std::string &target,
                                             bool &ok) noexcept {
    ok = true;
    auto &p = core.component(target);
    if ( (&p == &Customisation::Component::INVALID) || 
         (&p.entity() != &Customisation::Entity::INVALID) ) {
        LOGE("Invalid parameter provided '%s'", target.c_str());
        ok = false;
    }

    return p;
}

}  // namespace Customisation
