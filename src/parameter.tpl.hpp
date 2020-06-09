/**
 *
 * @file      src/parameter.tpl.hpp
 *
 * @brief     This is the template implementation of Customisation parameters
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

#include "customisation/io.hpp"
#include "customisation/parameter.hpp"
#include "types.hpp"

#include <string>
#include <vector>

#undef LOGTAG
#define LOGTAG "Customisation::Parameter"
#include "log.hpp"

namespace Customisation {

template <template <class, class> class T, template <class, class> class C, 
          template <class, class> class S, typename t>
Parameter<T, C, S, t>::Parameter() noexcept : 
    Component(type_of<typename T<t, void>::Type>(), 
              Trait::NONE, ~Trait::ENTITY), 
    T<t, void>(), C<t, void>(), S<t, void>() {
}

template <template <class, class> class T, template <class, class> class C, 
          template <class, class> class S, typename t>
Parameter<T, C, S, t>::Parameter(Type val) noexcept : 
    Component(type_of<typename T<t, void>::Type>(),
              Trait::NONE, ~Trait::ENTITY), 
    T<t, void>(), C<t, void>(), S<t, void>(std::move(val)) {
}

template <template <class, class> class T, template <class, class> class C, 
          template <class, class> class S, typename t>
Parameter<T, C, S, t>& Parameter<T, C, S, t>::operator = (Type val) noexcept {
    Error error = Error::NONE;
    val = C<t, void>::check(val, error);
    LOGD_IF_NOT(error == Error::NONE, "Invalid value provided for %s %s",
                type().c_str(), name().c_str());
    if (error == Error::NONE) {
        S<t, void>::store_value(std::move(val));
    }
    return *this;
}

template <template <class, class> class T, template <class, class> class C, 
          template <class, class> class S, typename t>
Parameter<T, C, S, t>::operator Type() const noexcept {
    return S<t, void>::retrieve_value();
}
        
template <template <class, class> class T, template <class, class> class C, 
          template <class, class> class S, typename t>
std::string Parameter<T, C, S, t>::value_to_string() const noexcept {
    return std::move(T<t, void>::to_str(S<t, void>::retrieve_value()));
}

template <template <class, class> class T, template <class, class> class C, 
          template <class, class> class S, typename t>
std::string Parameter<T, C, S, t>::values_to_string() const noexcept {

    std::string values(std::move(T<t, void>::signature()));
    Checker::Kind kind;
    auto value_set = C<t, void>::validity(kind);

    if (kind == Checker::Kind::NONE) {
        return values;
    }

    char separator;
    values += " ";
    if (kind == Checker::Kind::RANGE) {
        ASSERT(value_set.size() == 2, "Invalid range provided!");
        values    += "in [";
        separator  = ';';
    } else {
        if (kind == Checker::Kind::BLACK_LIST) {
            values += "not ";
        } 
        values += "in { ";
        separator   = ',';
    }
    bool first = true;
    for (auto const &value : value_set) {
        if (!first) {
            values += separator;
            values += ' ';
        }
        values += T<t, void>::to_str(value);
        first = false;
    }
    if (kind == Checker::Kind::RANGE) {
        values += ']';
    } else {
        values += " }";
    }

    return values;
}

template <template <class, class> class T, template <class, class> class C, 
          template <class, class> class S, typename t>
Error Parameter<T, C, S, t>::string_to_value(const std::string &val) noexcept {
    Error error = Error::NONE;
    std::list<std::string> tokens;
    Type value;

    error = IO::parse(val, tokens);
    if (error != Error::NONE) {
        return error;
    }

    if (tokens.size() == 1) {
        value = std::move(T<t, void>::from_str(tokens.front(), error));
    } else {
        value = std::move(T<t, void>::from_str(tokens, error));
    }
    if (error != Error::NONE) {
        return error;
    }
    
    value = C<t, void>::check(value, error);
    if (error != Error::NONE) {
        return error;
    }

    return S<t, void>::store_value(value);
}

}  // namespace Customisation
