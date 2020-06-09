/**
 *
 * @file      customisation/entity.hpp
 *
 * @brief     This is the definition of a Customisation entity
 *
 * @details   This file contains the definition of an entity. An entity is a
 *            specialised Customisation component that can encompass multiple
 *            other named entities and parameters, hence defining an adressable
 *            hierarchy of components through a dot-based text addressing. A
 *            Customisation entity is then the base class of a parametrisable
 *            class, hence the Parametrisable alias. In other words, defining
 *            a class as Parametrisable such as class MyClass : Parametrisable
 *            makes my class a parametrisable class where any parameter can be
 *            addressed from a text-based interface.
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

#include "customisation/component.hpp"
#include "customisation/error.hpp"
#include <functional>
#include <map>
#include <vector>

namespace Customisation {

class Entity : public Component {
    public:
        using Components = std::vector<std::reference_wrapper<Component> >;
        using Labels = std::vector<std::string>;
        
        static Entity            INVALID;
        static constexpr Entity &UNDEFINED = INVALID;
 
        explicit Entity(std::string kind) noexcept;
        /* An entity is always bound to other entities and parameters by
         * reference. It shall therefore not be moved (otherwise its reference 
         * will be broken) nor shall it be copied. */
        Entity(const Entity& other) = delete;
        Entity(Entity&& other) = delete;
        Entity& operator=(const Entity& other) = delete;
        Entity& operator=(Entity&& other) = delete;
        virtual ~Entity() noexcept;

        Labels labels() const noexcept;
        Components components() noexcept;
        const Components components() const noexcept;
        Components find(const std::string &kind) noexcept;
        Components find(const std::string &kind,
                        const std::string &name) noexcept;

        Labels entities() noexcept;
        Labels parameters() const noexcept;

        bool has(const std::string &path) const noexcept;
        Component &component(const std::string &path) noexcept;
        const Component &component(const std::string &path) const noexcept;

        void enable() noexcept;
        void enable(bool yes) noexcept;
        void disable() noexcept;
        void disable(bool yes) noexcept;
        bool enabled() const noexcept;
        bool disabled() const noexcept;

        Error initialise() noexcept;
        virtual Error setup() noexcept;
        
        void finalise() noexcept;
        virtual void terminate() noexcept;

        /* Set or configure the component at the given path */
        Error set(const std::string &path, const std::string &val) noexcept;
        Error configure(const std::string &path, 
                        const std::string &val) noexcept;

        /* Get the component value at the given path */
        Error get(const std::string &path, std::string &into) const noexcept;

        Error configure(const std::vector<std::string> &cfg,
                        int &nb) noexcept;
        std::vector<std::string> configured() const noexcept;
        std::vector<std::string> snapshot() const noexcept;
        std::vector<std::string> configurables() const noexcept;
        std::vector<std::string> setters() const noexcept;
        std::vector<std::string> getters() const noexcept;
        std::vector<std::string> structure() const noexcept;

    protected:
        Component &expose(Component &comp) noexcept;

        void dump(const std::string &base, std::vector<std::string> &record,
                  const int mode) const noexcept;

        /* Get the current value of the component */
        virtual std::string value_to_string() const noexcept final;

        /* Get all possible values for the component */
        virtual std::string values_to_string() const noexcept final;

        /* Set a new value for the component*/
        virtual Error string_to_value(const std::string &val) noexcept final;

    private:
        bool _enabled;
        void dump(const std::string &base, std::vector<std::string> &record,
                  const int mode, const Components &components) const noexcept;

        Components                                                _components;
        std::map<std::string, std::reference_wrapper<Component> > _map;
        const std::string                                         _kind;
};

}  // namespace Customisation

/* Define a Parametrisable class */
using Parametrisable = Customisation::Entity;
