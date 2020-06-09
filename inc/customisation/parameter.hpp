/**
 *
 * @file      customisation/parameter.hpp
 *
 * @brief     This is the definition of Customisation parameters
 *
 * @details   This file contains the definition of a parameter. A parameter is a
 *            leaf component, i.e. a component that does not have children and
 *            that contains a value by its own, be it a boolean, an integer, a
 *            real number, a string, a set of any of these, a vector of any of
 *            these, a map of any of these, a pair of any of these...
 *
 *            Each parameter is composed of a translator aimed at translating
 *            the string input as a value, a checker to ensure that the value is
 *            valid and a storage to define the actions to be taken when the
 *            value is updated.
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
#include "customisation/translator.hpp"
#include "customisation/checker.hpp"
#include "customisation/storage.hpp"

namespace Customisation {

template <template <class, class> class T, template <class, class> class C, 
          template <class, class> class S, typename t>
class Parameter : 
    public Component, public T<t, void>, public C<t, void>, public S<t, void> {
    public:
        using Type = t;
        
        Parameter() noexcept;
        Parameter(Type val) noexcept;
        /* A parameter is storing some data used by an entity, and it is bound
         * to the calling entity by reference. It shall therefore not be moved
         * (otherwise its reference will be broken) nor shall it be copied. */
        Parameter(const Parameter& other) = delete;
        Parameter(Parameter&& other) = delete;
        Parameter& operator=(const Parameter& other) = delete;
        Parameter& operator=(Parameter&& other) = delete;
        virtual ~Parameter() noexcept = default;

        Parameter& operator = (Type val) noexcept;
        operator Type() const noexcept;

    protected:
        /* Get the current value of the component */
        virtual std::string value_to_string() const noexcept override final;

        /* Get all possible values for the component */
        virtual std::string values_to_string() const noexcept override final;

        /* Set a new value for the component*/
        virtual Error string_to_value(const std::string &val) noexcept 
                                                              override final;
};

}  // namespace Customisation

/* A simple macro to ease the definition of a specific parameter */
#define PARAMETER(T, C, S, t) \
    Customisation::Parameter<Customisation::Translator::T, \
                             Customisation::Checker::C, \
                             Customisation::Storage::S, \
                             t>
