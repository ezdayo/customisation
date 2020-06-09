/**
 *
 * @file      customisation/component.hpp
 *
 * @brief     This is the definition of a Customisation component
 *
 * @details   This file contains the definition of a component. It is the base
 *            class of all Customisation objects be they entities or parameters.
 *            As such it provides the default query interface for handling any
 *            such object from a text-based interface. 
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

#include "customisation/error.hpp"
#include "customisation/traits.hpp"
#include <string>

/** This is the Customisation namespace */
namespace Customisation {

class Entity;

class Component {
    public:
        static Component            INVALID;
        static constexpr Component &UNDEFINED = INVALID;
 
        explicit Component(const std::string &type,
                           Trait base, Trait mask) noexcept;

        /* A component is namely a wrapper for storing information regarding
         * a hierarchy of components and parameters. As such it is safely
         * movable and copyable as long as the Entity where it is bound to
         * does not move */
        Component(const Component& other) = default;
        Component(Component&& other) = default;
        Component& operator=(const Component& other) = default;
        Component& operator=(Component&& other) = default;
        virtual ~Component() = default;

        Entity &entity() noexcept;
        const Entity &entity() const noexcept;
        const Entity *owner() const noexcept;

        Component &denominate(const std::string name) noexcept;
        Component &describe(const std::string desc) noexcept;
        Component &characterise(const Trait traits) noexcept;
        Component &bind(const Entity &owner) noexcept;

        const std::string &name() const noexcept;
        const std::string &description() const noexcept;
        const std::string &type() const noexcept;
        Trait traits() const noexcept;

        /* Set or configure the new component value */
        Error set(const std::string &val) noexcept;
        Error configure(const std::string &val) noexcept;

        /* Get the component value */
        Error get(std::string &into) const noexcept;
        std::string value() const noexcept;

        /* Get the component values */
        std::string values() const noexcept;

        /* Locking or unlocking a component for configuration */
        bool locked() const noexcept;
        bool unlocked() const noexcept;
        void lock() noexcept;
        void unlock() noexcept;

    protected:

        /* The following 3 virtual methods must be overridden by the child
         * classes */

        /* Get the current value of the component */
        virtual std::string value_to_string() const noexcept;

        /* Get all possible values for the component */
        virtual std::string values_to_string() const noexcept;

        /* Set a new value for the component*/
        virtual Error string_to_value(const std::string &val) noexcept;
    
    private:
        std::string        _name;
        std::string        _description;
        const std::string& _type;
        Trait              _traits;
        Trait              _mask;
        const Entity *     _owner;
};

}  // namespace Customisation
