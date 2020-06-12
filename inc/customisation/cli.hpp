/**
 *
 * @file      customisation/cli.hpp
 *
 * @brief     This is the definition of a Customisation command line interface
 *
 * @details   This file describes the structure of a Customisation CLI aimed at
 *            interacting with a Parametrisable entity at runtime.
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

#pragma once

#include <list>
#include <map>
#include <string>

#include "customisation/entity.hpp"

namespace Customisation {

class CLI final {
    public:
        explicit CLI(Customisation::Entity &c) noexcept;

        /** CLI cannot be copied nor moved */
        CLI(const CLI& other) = delete;
        CLI(CLI&& other) = delete;
        CLI& operator=(const CLI& other) = delete;
        CLI& operator=(CLI&& other) = delete;
        virtual ~CLI() noexcept = default;

        /* Helper to get a component reference from a target */
        Customisation::Component &get_component(const std::string &target,
                                                bool &ok) noexcept;

        /* Helper to get an entity reference from a target */
        Customisation::Entity &get_entity(const std::string &target, 
                                          bool &ok) noexcept;

        /* Helper to get a parameter reference from a target */
        Customisation::Component &get_parameter(const std::string &target,
                                                bool &ok) noexcept;

        /* Interactive mode */
        void interactive() noexcept;
        
        /* Script mode */
        void script(const std::string &file) noexcept;

    private:
        bool cmd_exit(std::list<std::string> &tokens) noexcept;
        bool cmd_finalise(std::list<std::string> &tokens) noexcept;
        bool cmd_get(std::list<std::string> &tokens) noexcept;
        bool cmd_initialise(std::list<std::string> &tokens) noexcept;
        bool cmd_load(std::list<std::string> &tokens) noexcept;
        bool cmd_prompt(std::list<std::string> &tokens) noexcept;
        bool cmd_run(std::list<std::string> &tokens) noexcept;
        bool cmd_save(std::list<std::string> &tokens) noexcept;
        bool cmd_show(std::list<std::string> &tokens) noexcept;
        bool cmd_set(std::list<std::string> &tokens) noexcept;
        bool cmd_wait(std::list<std::string> &tokens) noexcept;
        
        bool parse(std::string &data) noexcept;
       
        /* The DScribe core engine */
        Customisation::Entity &                                   core;
  
        /* The available CLI commands */ 
        std::map<std::string,
                 std::function<bool (std::list<std::string> &)> > command;
};

}  // namespace Customisation
