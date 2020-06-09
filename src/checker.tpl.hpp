/**
 *
 * @file      src/checker.tpl.hpp
 *
 * @brief     This is the template implementation of all kinds of parameter
 *            checkers
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

#include "customisation/checker.hpp"
#include "customisation/component.hpp"
#include "customisation/entity.hpp"

#include <limits>
#include <set>

#undef LOGTAG
#define LOGTAG "Customisation::Checker"
#include "log.hpp"

namespace Customisation {
namespace Checker {

template <typename T, typename Default>
std::set<T> None<T, Default>::empty = {};

template <typename T, typename Default>
T None<T, Default>::check(T val, Error &/*error*/) const noexcept {
    return std::move(val); 
}

template <typename T, typename Default>
const std::set<T> &None<T, Default>::validity(Kind &kind) const noexcept {
    kind = Kind::NONE;
    return empty;
}

template <typename T, typename Default>
std::set<T> WhiteListed<T, Default>::empty = {};

template <typename T, typename Default>
void WhiteListed<T, Default>::allow() noexcept {
    _allowed.clear();
}

template <typename T, typename Default>
void WhiteListed<T, Default>::allow(T value) noexcept {
    _allowed.emplace(std::move(value));
}

template <typename T, typename Default>
void WhiteListed<T, Default>::allow(std::set<T> values) noexcept {
    /* Passing an empty set it clearing the whole list */
    if (!values.empty()) { 
        for (auto const &value: values) {
            allow(std::move(value));
        }
    } else {
        allow();
    }
}

template <typename T, typename Default>
void WhiteListed<T, Default>::forbid(const T &value) noexcept {
    auto const &it = _allowed.find(value);
    LOGD_IF_NOT((it != _allowed.end()),
                "Removing a non existing element in white-list!");
    if (it != _allowed.end()) {
        _allowed.erase(it);
    }
}

template <typename T, typename Default>
void WhiteListed<T, Default>::forbid(const std::set<T> &values) noexcept {
    for (auto const &value: values) {
        forbid(value);
    }
}

template <typename T, typename Default>
T WhiteListed<T, Default>::check(T value, Error &error) const noexcept {
    /* Consider a white-list with no entries as a no-checker input */
    if ((!_allowed.empty()) && (_allowed.find(value) == _allowed.end())) {
        error = Error::INVALID_VALUE;
    }
    return std::move(value); 
}

template <typename T, typename Default>
const std::set<T> &WhiteListed<T, Default>::validity(Kind &kind) const noexcept {
    /* Only consider a white-list if it is not empty! */
    if (!_allowed.empty()) {
        kind = Kind::WHITE_LIST;
        return _allowed;
    }

    kind = Kind::NONE;
    return empty;
}

template <typename T>
T WhiteListed<T, when_container(T)>::check(T values, Error &error) 
    const noexcept {
    for (auto it = values.begin(); it != values.end(); /**/) {
        if (WhiteListed<Type>::_allowed.find(*it) == 
            WhiteListed<Type>::_allowed.end()) {
            if (!type::is_associative_container<T>::value) {
                error = Error::INVALID_VALUE;
            }
            it = values.erase(it);
        } else {
            ++it;
        }
    }
    return std::move(values); 
}

template <typename T, typename Default>
void BlackListed<T, Default>::allow(const T &value) noexcept {
    auto const &it = _forbidden.find(value);
    LOGD_IF_NOT((it != _forbidden.end()),
                 "Removing a non existing element in black-list!");
    if (it != _forbidden.end()) {
        _forbidden.erase(it);
    }
}

template <typename T, typename Default>
void BlackListed<T, Default>::allow(const std::set<T> &values) noexcept {
    for (auto const &value: values) {
        allow(value);
    }
}

template <typename T, typename Default>
void BlackListed<T, Default>::forbid() noexcept {
    _forbidden.clear();
}

template <typename T, typename Default>
void BlackListed<T, Default>::forbid(T value) noexcept {
    _forbidden.emplace(std::move(value));
}

template <typename T, typename Default>
void BlackListed<T, Default>::forbid(std::set<T> values) noexcept {
    /* Passing an empty set it clearing the whole list */
    if (!values.empty()) { 
        for (auto const &value: values) {
            forbid(std::move(value));
        }
    } else {
        forbid();
    }
}

template <typename T, typename Default>
T BlackListed<T, Default>::check(T value, Error &error) const noexcept {
    if (_forbidden.find(value) != _forbidden.end()) {
        error = Error::INVALID_VALUE;
    }
    return std::move(value); 
}

template <typename T, typename Default>
const std::set<T> &BlackListed<T, Default>::validity(Kind &kind) const noexcept {
    kind = Kind::BLACK_LIST;
    return _forbidden;
}

template <typename T>
T BlackListed<T, when_container(T)>::check(T values, Error &error) 
    const noexcept {
    for (auto it = values.begin(); it != values.end(); /**/) {
        if (BlackListed<Type>::_forbidden.find(*it) != 
            BlackListed<Type>::_forbidden.end()) {
            if (!type::is_associative_container<T>::value) {
                error = Error::INVALID_VALUE;
            }
            it = values.erase(it);
        } else {
            ++it;
        }
    }
    return std::move(values); 
}

template <typename T>
Bounded<T, when_arithmetic(T)>::Bounded() noexcept : 
    _min(std::numeric_limits<T>::lowest()), 
    _max(std::numeric_limits<T>::max()), _range( {_min, _max} ) { }

template <typename T>
Error Bounded<T, when_arithmetic(T)>::range(T min, T max) noexcept {
    if (min >= max) {
        return Error::INVALID_RANGE;
    }
    _min   = min;
    _max   = max;
    _range = { min, max };
    return Error::NONE;
}

template <typename T>
T Bounded<T, when_arithmetic(T)>::min() const noexcept {
    return _min;
}

template <typename T>
T Bounded<T, when_arithmetic(T)>::max() const noexcept {
    return _max;
}

template <typename T>
T Bounded<T, when_arithmetic(T)>::check(T value, Error &error)
    const noexcept {
    if ( (value < _min) || (value > _max) ) {
        error = Error::INVALID_VALUE;
    }
    return std::move(value); 
}

template <typename T>
const std::set<T> &Bounded<T, when_arithmetic(T)>::validity(Kind &kind) 
    const noexcept {
    kind = Kind::RANGE;
    return _range;
}

template <typename T>
T Bounded<T, when_arithmetic_container(T)>::check(T values, Error &error)
    const noexcept {
    for (auto it = values.begin(); it != values.end(); /**/) {
        if ( (*it < Bounded<Type>::_min) || (*it > Bounded<Type>::_max) ) {
            if (!type::is_associative_container<T>::value) {
                error = Error::INVALID_VALUE;
            }
            it = values.erase(it);
        } else {
            ++it;
        }
    }
    return std::move(values); 
}

template <typename T>
T Saturating<T, when_arithmetic(T)>::check(T value,
                                           Error &/*error*/) const noexcept {
    if (value < Bounded<T, when_arithmetic(T)>::_min) {
        value = Bounded<T, when_arithmetic(T)>::_min;
    } else if (value > Bounded<T, when_arithmetic(T)>::_max) {
        value = Bounded<T, when_arithmetic(T)>::_max;
    }
    return std::move(value); 
}

template <typename T>
T Saturating<T, when_set(T)>::check(T values, Error &error) const noexcept {
    for (auto it = values.begin(); it != values.end(); ) {
        auto saturated = Saturating<Type>::check(*it, error);
        if (saturated != *it) {
            values.emplace_hint(it, saturated);
            it = values.erase(it);
        } else {
            ++it;
        }
    }
    return std::move(values); 
}

template <typename T>
T Saturating<T, when_vector(T)>::check(T values, Error &error)
    const noexcept {
    for (auto & v : values) {
        v = Saturating<Type>::check(v, error);
    }
    return std::move(values); 
}

}  // namespace Checker
}  // namespace Customisation
