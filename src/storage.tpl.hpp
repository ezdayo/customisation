/**
 *
 * @file      src/storage.cpp
 *
 * @brief     This is the template implementation of Customisation parameter
 *            storage classes
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
#include "customisation/entity.hpp"
#include "customisation/storage.hpp"
#include "types.hpp"

#include <map>
#include <set>
#include <vector>

#undef LOGTAG
#define LOGTAG "Customisation::Storage"
#include "log.hpp"

namespace Customisation {
namespace Storage {

template <typename T, typename Default>
Immediate<T, Default>::Immediate() noexcept { }

template <typename T, typename Default>
Immediate<T, Default>::Immediate(T val) noexcept :
_current_value(std::move(val)) { }

template <typename T, typename Default>
Error Immediate<T, Default>::store_value(T val) noexcept {
        _current_value = std::move(val);
        return Error::NONE;
}

template <typename T, typename Default>
const T &Immediate<T, Default>::retrieve_value() const noexcept {
    return _current_value;
}

template <typename T, typename Default>
Callable<T, Default>::Callable() noexcept {}
 
template <typename T, typename Default>
Callable<T, Default>::Callable(T val) noexcept : 
Immediate<T, Default>::Immediate(std::move(val)) { }

template <typename T, typename Default>
void Callable<T, Default>::trigger(Callbackable on_update) noexcept {
    _on_update = std::move(on_update);
}

template <typename T, typename Default>
Error Callable<T, Default>::store_value(T val) noexcept {
    /* Call the callback after the value has been stored so that it can
     * be accessed in the callback function */
    auto error = Immediate<T, Default>::store_value(std::move(val));
    if (error != Error::NONE) {
        return error;
    }

    if (_on_update != nullptr) {
        error = _on_update(Immediate<T, Default>::_current_value);
    }

    return error;
}

template <typename T, typename Default>
ReadOnly<T, Default>::ReadOnly() noexcept {
    /* Shall never happen ! */
    ASSERT(false, "A read-only paramater shall be initialised!");
}

template <typename T, typename Default>
ReadOnly<T, Default>::ReadOnly(T val) noexcept {
    _current_value = std::move(val);
}

template <typename T, typename Default>
void ReadOnly<T, Default>::update(T val) noexcept { 
    _current_value = std::move(val);
}
 
template <typename T, typename Default>
Error ReadOnly<T, Default>::store_value(T /*val*/) noexcept {
    /* Shall never happen ! */
    ASSERT(false, "Cannot change the value of a read-only parameter!");
    return Error::INVALID_REQUEST;
}

template <typename T, typename Default>
const T &ReadOnly<T, Default>::retrieve_value() const noexcept {
    return _current_value; 
}

template <typename T, typename Default>
Deferred<T, Default>::Deferred() noexcept { }

template <typename T, typename Default>
Deferred<T, Default>::Deferred(T val) noexcept {
    _next_value = val;
    _current_value = std::move(val);
}
       
template <typename T, typename Default>
void Deferred<T, Default>::update() noexcept { _current_value = _next_value; }
 
template <typename T, typename Default>
Error Deferred<T, Default>::store_value(T val) noexcept {
    _next_value = std::move(val);
    return Error::NONE;
}

template <typename T, typename Default>
const T &Deferred<T, Default>::retrieve_value() const noexcept {
    return _current_value;
}

}  // namespace Storage
}  // namespace Customisation
