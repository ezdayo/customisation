/**
 *
 * @file      customisation/storage.hpp
 *
 * @brief     This is the definition of a Customisation parameter storage class
 *
 * @details   This file contains the definition of a parameter storage class. A
 *            parameter storage can be any of the following:
 *            - Immediate for being updated immediatly;
 *            - Callable for triggering an action when updated;
 *            - ReadOnly for defining a read-only parameter;
 *            - Deferred for a deferred update parameter. 
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

#include <functional>
#include <type_traits>
		
#include "customisation/error.hpp"

namespace Customisation {
namespace Storage {

template <typename T, typename Default = void>
class Immediate {
    public:
        using Type = T;
        Immediate() noexcept;
        Immediate(Type val) noexcept;

    protected:
        Error store_value(Type val) noexcept;
        const Type &retrieve_value() const noexcept;

        Type _current_value;
};


template <typename T, typename Default = void>
class Callable : public Immediate<T, Default> {

    /* Helper class to allow type deduction on templated member arguments */
    template<class D> struct SameAs {
        typedef D type;
    };

    public:
        using Type = T;
        using Callback = std::function<Error(const Type&)>;
        using Callbackable = typename SameAs<Callback>::type;

        Callable() noexcept;
        Callable(Type val) noexcept;

        void trigger(Callbackable on_update) noexcept;

    protected:
        Error store_value(Type val) noexcept;

        Callback _on_update;
};

template <typename T, typename Default = void>
class ReadOnly {
    public:
        using Type = T;
        ReadOnly() noexcept;
        ReadOnly(Type val) noexcept;
        
        void update(Type val) noexcept;

    protected:
        Error store_value(Type val) noexcept;
        const Type &retrieve_value() const noexcept;

        Type _current_value;
};

template <typename T, typename Default = void>
class Deferred {
    public:
        using Type = T;
        Deferred() noexcept;
        Deferred(Type val) noexcept;
       
        void update() noexcept;
    
    protected:
        Error store_value(Type val) noexcept;
        const Type &retrieve_value() const noexcept;

        Type _current_value;
        Type _next_value;
};

}  // namespace Storage
}  // namespace Customisation
