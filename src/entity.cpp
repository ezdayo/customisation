/**
 *
 * @file      src/entity.cpp
 *
 * @brief     This is the implementation of a Customisation entity
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

#include "customisation/entity.hpp"
#include "customisation/io.hpp"
#include "customisation/traits.hpp"
#include <cctype>
#include <iostream>
#include <regex>
#include <sstream>

#undef LOGTAG
#define LOGTAG "Customisation::Entity"
#include "log.hpp"

#include "types.hpp"

namespace Customisation {

static const std::string invalid("Invalid");

Entity Entity::INVALID(invalid);

Entity::Entity(std::string kind) noexcept :
    Component(type_of<Entity>(), Trait::ENTITY, Trait::NONE), _enabled(true), 
    _components(), _map(), _kind(std::move(kind)) {
    /* An entity cannot be altered per-se,
     * only its components can be modified */
    characterise(Trait::NONE);
}

Entity::~Entity() noexcept {
    /* Call terminate unconditionally to ensure a proper exit */
    terminate();
}

Entity::Labels Entity::labels() const noexcept {
    Labels keys;
    keys.reserve(_components.size());

    for (auto const& entry : _map) {
        keys.emplace_back(entry.first);
    }
  
    return keys;
}

Entity::Components Entity::components() noexcept {
    Components comps;
    comps.reserve(_components.size());

    for (auto & entry : _map) {
        comps.emplace_back(entry.second);
    }
  
    return comps;
}
   
const Entity::Components Entity::components() const noexcept {
    Components comps;
    comps.reserve(_components.size());

    for (auto const& entry : _map) {
        comps.emplace_back(entry.second);
    }
  
    return comps;
}
   
Entity::Components Entity::find(const std::string &kind) noexcept {
    return find(kind, "*");
}

Entity::Components Entity::find(const std::string &kind,
                                const std::string &name) noexcept {
    Entity::Components comps;
    std::regex rkind(kind.c_str());
    std::regex rname(name.c_str());

    if ((std::regex_match(values(), rkind)) &&
        (std::regex_match(this->name(), rname)) ) {
            comps.emplace_back(*this);
    }

    for (auto const &comp : _components) {
        if ( (std::regex_match(comp.get().values(), rkind)) &&
             (std::regex_match(comp.get().name(), rname)) ) {
            comps.emplace_back(comp);
        }
    }

    return comps;
}

Entity::Labels Entity::entities() noexcept {
    Entity::Labels ents;
    for (auto &compo : components()) {
        auto &entity = compo.get().entity();
        if (&entity != &Entity::INVALID) {
            const auto &name = compo.get().name();
            ents.push_back(name);
            for (const auto &inside : entity.entities()) {
                ents.push_back(name + "." + inside);
            }
        }
    }

    return ents;
}

Entity::Labels Entity::parameters() const noexcept {
    Entity::Labels params;
  
    for (auto &compo : components()) {
        auto &entity = compo.get().entity();
        const auto &name = compo.get().name();
        if (&entity != &Entity::INVALID) {
            for (const auto &inside : entity.parameters()) {
                params.push_back(name + "." + inside);
            }
        } else {
            params.push_back(name);
        }
    }

    return params;
}

bool Entity::has(const std::string &path) const noexcept {
    return (&component(path) != &Component::INVALID);
}
   
const Component &Entity::component(const std::string &path) const noexcept {
    std::istringstream  spath(path);
    std::string         key;

    /* Special case of the "*" path */
    if (path == "*") {
        return *this;
    }

    /* The key is any set of characters until a dot is found or the end of the
     * stream is reached */
    std::getline(spath, key, '.');

    auto found = _map.find(key);
    if (found != _map.end()) {
        const Component& compo = found->second;
        if (spath.eof()) {
            return compo;
        }

        auto &entity = compo.entity();
        if (&entity == &Entity::INVALID) {
            return Component::INVALID;
        }

        std::getline(spath, key);
        return entity.component(key);
    }
            
    return Component::INVALID;
}

Component &Entity::component(const std::string &path) noexcept {
    std::istringstream  spath(path);
    std::string         key;

    /* Special case of the "*" path */
    if (path == "*") {
        return *this;
    }

    /* The key is any set of characters until a dot is found or the end of the
     * stream is reached */
    std::getline(spath, key, '.');

    auto found = _map.find(key);
    if (found != _map.end()) {
        Component& compo = found->second;
        if (spath.eof()) {
            return compo;
        }

        auto &entity = compo.entity();
        if (&entity == &Entity::INVALID) {
            return Component::INVALID;
        }

        std::getline(spath, key);
        return entity.component(key);
    }
            
    return Component::INVALID;
}

void Entity::enable() noexcept { 
    enable(true); 
}

void Entity::enable(bool yes) noexcept { 
    _enabled = yes; 
}

void Entity::disable() noexcept {
    enable(false);
}

void Entity::disable(bool yes) noexcept {
    enable(!yes);
}

bool Entity::enabled() const noexcept {
    return _enabled;
}

bool Entity::disabled() const noexcept {
    return !_enabled;
}

Error Entity::initialise() noexcept {

    for (auto & entry : _components) {
        auto &entity = entry.get().entity();
        if ( (&entity != &Entity::INVALID) && (entity.enabled()) ) {
            auto error = entity.initialise();
            if (error != Error::NONE) {
                LOGE("%s[%s]::initialise() error %d",
                      type().c_str(), name().c_str(), static_cast<int>(error));
                return error;
            }
        }
    }

    /* Lock components after they have been setup so that any configuration 
     * update due to the setup process can be taken into account */
    for (auto & entry : _components) {
        entry.get().lock();
    }
    lock();

    return setup();
}

Error Entity::setup() noexcept {
    return Error::NONE; 
}

Error Entity::flush() noexcept {

    for (auto & entry : _components) {
        auto &entity = entry.get().entity();
        if ( (&entity != &Entity::INVALID) && (entity.enabled()) ) {
            auto error = entity.clear();
            if (error != Error::NONE) {
                LOGE("%s[%s]::flush() error %d",
                      type().c_str(), name().c_str(), static_cast<int>(error));
                return error;
            }
        }
    }

    return clear();
}

Error Entity::clear() noexcept {
    return Error::NONE; 
}

void Entity::finalise() noexcept {

    /* Unlock all components first, so that they can be optionally be 
     * reconfigured during the termination */
    for (auto & entry : _components) {
        entry.get().unlock();
    }
    unlock();

    /* Terminate all components in reverse order from their initialisation */
    terminate();
    for (auto it = _components.rbegin(); it != _components.rend(); ++it) {
        auto &entity = it->get().entity();
        if ( (&entity != &Entity::INVALID) && (entity.enabled()) ) {
            entity.finalise();
        }
    }

}

void Entity::terminate() noexcept {
}

Error Entity::set(const std::string &path, const std::string &val) noexcept {
    auto &compo = component(path);
    if (&compo == &Component::INVALID) {
        return Error::NOT_EXISTING;
    }
    return compo.set(val);
}
        
Error Entity::configure(const std::string &path, 
                        const std::string &val) noexcept {
    auto &compo = component(path);
    if (&compo == &Component::INVALID) {
        return Error::NOT_EXISTING;
    }
    return compo.configure(val);
}

Error Entity::get(const std::string &path, std::string &into) const noexcept {
    auto &compo = component(path);
    if (&compo == &Component::INVALID) {
        return Error::NOT_EXISTING;
    }
    return compo.get(into);
}

Error Entity::configure(const std::vector<std::string> &cfg, int &nb) noexcept {
    std::list<std::string> token;
    Error                  error = Error::NONE;

    for (auto const &line : cfg) {
        ++nb;

        error = IO::parse(line, token, 3, 3);
        if (error != Error::NONE) {
            break;
        }

        if (token.empty()) {
            continue;
        }

        error = configure(token.front(), token.back());
        if (error != Error::NONE) {
            break;
        }

        token.pop_front();
        if (token.front().compare("=") != 0) {
            return Error::INVALID_VALUE;
        }

        token.clear();
    }

    return error;
}

static constexpr const int MODE_SNAPSHOT      = 0;
static constexpr const int MODE_CONFIGURED    = 1;
static constexpr const int MODE_CONFIGURABLES = 2;
static constexpr const int MODE_SETTERS       = 3;
static constexpr const int MODE_GETTERS       = 4;
static constexpr const int MODE_STRUCTURE     = 5;

std::vector<std::string> Entity::configured() const noexcept {
    std::vector<std::string> record;
    dump("", record, MODE_CONFIGURED);
    return record;
}

std::vector<std::string> Entity::snapshot() const noexcept {
    std::vector<std::string> record;
    dump("", record, MODE_SNAPSHOT);
    return record;
}

std::vector<std::string> Entity::configurables() const noexcept {
    std::vector<std::string> record;
    dump("", record, MODE_CONFIGURABLES);
    return record;
}
        
std::vector<std::string> Entity::setters() const noexcept {
    std::vector<std::string> record;
    dump("", record, MODE_SETTERS);
    return record;
}
        
std::vector<std::string> Entity::getters() const noexcept {
    std::vector<std::string> record;
    dump("", record, MODE_GETTERS);
    return record;
}
        
std::vector<std::string> Entity::structure() const noexcept {
    std::vector<std::string> record;
    dump("", record, MODE_STRUCTURE);
    return record;
}
        
Component &Entity::expose(Component &comp) noexcept {
    ASSERT((!comp.name().empty()),
            "Cannot expose an anonymous component (%s)", name().c_str());
    ASSERT((_map.find(comp.name()) == _map.end()),
           "Already have a component named %s (%s)", 
           comp.name().c_str(), name().c_str());
    ASSERT((comp.owner() == nullptr),
           "Component %s already in enitity %s (%s)", 
           comp.name().c_str(), comp.owner()->name().c_str(), name().c_str());
    
    if ( (&comp != &Component::INVALID) &&
         (!comp.name().empty()) &&
         (_map.find(comp.name()) == _map.end()) &&
         (comp.owner() == nullptr) ) {
        _components.emplace_back(comp);
        _map.emplace(comp.name(), comp);

        return comp.bind(*this);
    }

    return Component::INVALID;
}

void Entity::dump(const std::string &base, std::vector<std::string> &record,
                  const int mode) const noexcept {

    if (_components.empty()) {
        return;
    }

    /* Use the entity configuration order when issuing the sequence of
     * configuration (for configured and configuration), otherwise, to describe
     * the interfaces, an alphabetical order is best */
    if ( (mode == MODE_CONFIGURED) || (mode == MODE_CONFIGURABLES) ) {
        dump(base, record, mode, _components);
    } else {
        dump(base, record, mode, components());
    }
}

std::string Entity::value_to_string() const noexcept {
    return _kind;
}
        
std::string Entity::values_to_string() const noexcept {
    return _kind;
}
        
Error Entity::string_to_value(const std::string &/*val*/) noexcept {
    return Error::INVALID_REQUEST;
}

void Entity::dump(const std::string &base, std::vector<std::string> &record,
                  const int mode, const Entity::Components &comps) 
                  const noexcept {
  
    /* Structure mode is for entities only, whereas other modes are for
     * parameters */
    Trait mask  = Trait::UNDEFINED | Trait::ENTITY;
    Trait match = Trait::NONE;

    switch (mode) {
        case MODE_CONFIGURED:
        case MODE_CONFIGURABLES:
            mask  |= Trait::CONFIGURABLE;
            match |= Trait::CONFIGURABLE;
            break;
        case MODE_SETTERS:
            mask  |= Trait::SETTABLE;
            match |= Trait::SETTABLE;
            break;
        case MODE_STRUCTURE:
            match |= Trait::ENTITY;
            break;
    }

    std::string root(base);
    if (base.size() > 0) {
        root.append(".");
    }

    for (auto const comp : comps) {
        if ( (comp.get().traits() & mask) == match) {
     
            std::string path(root+comp.get().name());
            switch (mode) {
                case MODE_CONFIGURED:
                case MODE_SNAPSHOT:
                    Customisation::IO::compose(record, std::move(path), 
                                               comp.get().value(), "", "");
                    break;
                case MODE_CONFIGURABLES:
                case MODE_SETTERS:
                case MODE_GETTERS:
                case MODE_STRUCTURE:
                    Customisation::IO::compose(record, std::move(path), 
                                               comp.get().type(),
                                               comp.get().values(),
                                               comp.get().description());
                    break;
            }
        }

        auto &entity = comp.get().entity();
        if (&entity != &Entity::INVALID) {
            entity.dump(root+comp.get().name(), record, mode);
        }
    }
}
        
}  // namespace Customisation
