/**
 *
 * @file      src/translator.cpp
 *
 * @brief     This is the templated implementation of Customisation parameter
 *            translator classes
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
#include "customisation/translator.hpp"
#include "types.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <ios> // Even for Android !!!! :-D
#include <iterator>
#include <limits>
#include <sstream>

#undef LOGTAG
#define LOGTAG "Customisation::Translator"
#include "log.hpp"

namespace Customisation {
namespace Translator {

static Error too_many_tokens_in(std::list<std::string> &tokens) noexcept {
        LOGD("Expected a single string argument, got %lu tokens!",
             (unsigned long) tokens.size());
        return Error::INVALID_VALUE;
}

static Error too_few_tokens() noexcept {
        LOGD("Expected multiple tokens, only got one!");
        return Error::INVALID_VALUE;
}

template <typename T> 
T None<T, when_string(T)>::from_str(std::list<std::string> &tokens,
                                    Error &error) const noexcept {
        error = too_many_tokens_in(tokens);
        return "";
 }

template <typename T> 
T None<T, when_string(T)>::from_str(std::string &token,
                                    Error &/*error*/) const noexcept {
    return std::move(token); 
}

template <typename T> 
std::string None<T, when_string(T)>::to_str(const T &val)
    const noexcept {
    return val;
}

template <typename T> 
std::string None<T, when_string(T)>::signature()
    const noexcept {
    return "string id";
}

template <typename T> 
T Direct<T, when_string(T)>::from_str(std::list<std::string> &tokens,
                                      Error &error) const noexcept {
        error = too_many_tokens_in(tokens);
        return "";
 }

template <typename T> 
T Direct<T, when_string(T)>::from_str(std::string &token,
                                      Error &error) const noexcept {
    auto f = token.front();
    auto b = token.back();
    if ( (f == b) && ( (f != '"') || (f != '\'')) ) {
        return std::move(token.substr(1, token.size()-2)); 
    }

    /* Accept non quoted strings only if they do not contain spaces ! */
    for (auto c : token) {
        if (std::isspace(c)) {
            error = Error::INVALID_VALUE;
            return "";
        }
    }

    return std::move(token);
}

template <typename T> 
std::string Direct<T, when_string(T)>::to_str(const T &val) const noexcept {
    std::string str("\"");
    /* Assume that at most a couple of backslash might be necessary */
    str.reserve(val.size()+4);
    for (auto c : val) {
        if ( (c == '"') || (c == '\\')) {
            str += '\\';
        }
        str += c;
    }
    str += '"';

    return str;
}

template <typename T> 
std::string Direct<T, when_string(T)>::signature() const noexcept {
    return type_of<T>();
}

static const std::array<std::string, 4> bool_true  
                                    { { "1", "true",  "yes", "on"  } };
static const std::array<std::string, 4> bool_false
                                    { { "0", "false", "no",  "off" } };

template <typename T>
Direct<T, when_is(T, bool)>::Direct() noexcept : fmt(0) { }

template <typename T>
void Direct<T, when_is(T, bool)>::use(BoolFormat format) noexcept {
    auto id = static_cast<unsigned int>(format);
    if ((id < 0) || (id >= bool_true.size())) {
        LOGW("Invalid boolean format provided %d!", id);
        id = 0;
    }
    fmt = id;
}

template <typename T> 
T Direct<T, when_is(T, bool)>::from_str(std::list<std::string> &tokens,
                                        Error &error) const noexcept {
        error = too_many_tokens_in(tokens);
        return false;
 }

template <typename T> 
T Direct<T, when_is(T, bool)>::from_str(std::string &token,
                                        Error &error) const noexcept {
    std::transform(token.begin(), token.end(), token.begin(), ::tolower);

    auto it = std::find(bool_true.begin(), bool_true.end(), token);
    if (it != bool_true.end()) {
        return true;
    }

    it = std::find(bool_false.begin(), bool_false.end(), token);
    if (it != bool_false.end()) {
        return false;
    }

    error = Error::INVALID_VALUE;
    return false;
}

template <typename T> 
std::string Direct<T, when_is(T, bool)>::to_str(const T &val) const noexcept {
    if (val) {
        return bool_true[fmt];
    } else {
        return bool_false[fmt];
    }
}
 
template <typename T> 
std::string Direct<T, when_is(T, bool)>::signature() const noexcept {
    return type_of<T>() + " in { " + bool_false[fmt] + ", " + 
           bool_true[fmt] + " }";
}

static const std::array<std::ios::fmtflags, 3> integral_flags  
                        { { std::ios::dec, std::ios::hex,  std::ios::oct } };

static const std::array<std::string, 3> integral_desc  
                        { { "dec", "hex", "oct" } };

template <typename T>
Direct<T, when_integral(T)>::Direct() noexcept : fmt(0) { }

template <typename T>
void Direct<T, when_integral(T)>::use(NumberFormat format) noexcept {
    auto id = static_cast<unsigned int>(format);
    if ((id < 0) || (id >= integral_flags.size())) {
        LOGW("Invalid integer format provided %d!", id);
        id = 0;
    }
    fmt = id;
}

template <typename T> 
T Direct<T, when_integral(T)>::from_str(std::list<std::string> &tokens,
                                        Error &error) const noexcept {
        error = too_many_tokens_in(tokens);
        return 0;
}

template <typename T> 
T Direct<T, when_integral(T)>::from_str(std::string &token,
                                        Error &error) const noexcept {
    T value(0);
    std::istringstream data(token);
    
    data.setf(integral_flags[fmt], std::ios::basefield);
    data.setf(std::ios::showbase|std::ios::uppercase);

    if (sizeof(T) == 1) {
        /* Stringstream do not handle 8-bit integer correctly. Those are
         * considered as characters. So we'll extract a 16-bit integer instead
         * and cast its value to the 8-bit counterpart, setting the error bit
         * too ! */
        int16_t wider_value;
        data >> wider_value;

        if (wider_value < static_cast<int16_t>(std::numeric_limits<T>::min())) {
            value = std::numeric_limits<T>::min();
            error = Error::INVALID_VALUE;
        } else if (wider_value > 
                   static_cast<int16_t>(std::numeric_limits<T>::max())) {
            value = std::numeric_limits<T>::max();
            error = Error::INVALID_VALUE;
        } else {
            value = static_cast<T>(wider_value);
        }
    } else {
        data >> value;
    }

    auto status = data.rdstate();
    if (status != (std::ios::goodbit|std::ios::eofbit)) {
        error = Error::INVALID_VALUE;
    }
    return value;
}

template <typename T> 
std::string Direct<T, when_integral(T)>::to_str(const T &val) const noexcept {
    std::ostringstream data;
    
    data.setf(integral_flags[fmt], std::ios::basefield);
    data.setf(std::ios::showbase|std::ios::uppercase);
    if (sizeof(T) == 1) {
        /* Stringstream do not handle 8-bit integer correctly. Those are
         * considered as characters. So we'll extract a 16-bit integer instead
         * and cast its value to the 8-bit counterpart, setting the error bit
         * too ! */
        data << static_cast<int16_t>(val);
    } else {
        data << val;
    }

    return data.str();
}
 
template <typename T> 
std::string Direct<T, when_integral(T)>::signature() const noexcept {
    return integral_desc[fmt] + ". " + type_of<T>();
}

template <typename T>
Direct<T, when_floating_point(T)>::Direct() noexcept : 
                        fmt(std::numeric_limits<T>::digits10 + 1) { }

template <typename T>
void Direct<T, when_floating_point(T)>::precision(int prec) noexcept {
    if (prec < 0) {
        prec = 0;
    }
    if (prec > std::numeric_limits<T>::digits10 + 1) {
        prec = std::numeric_limits<T>::digits10 + 1;
    }

    fmt = prec;
}

template <typename T> 
T Direct<T, when_floating_point(T)>::from_str(std::list<std::string> &tokens,
                                              Error &error) const noexcept {
        error = too_many_tokens_in(tokens);
        return 0;
 }

template <typename T> 
T Direct<T, when_floating_point(T)>::from_str(std::string &token,
                                              Error &error) const noexcept {
    T value;
    std::istringstream data(token);
    
    data >> value;

    auto status = data.rdstate();
    if (status != (std::ios::goodbit|std::ios::eofbit)) {
        error = Error::INVALID_VALUE;
    }
    return value;
}

template <typename T> 
std::string Direct<T, when_floating_point(T)>::to_str(const T &val)
    const noexcept {
    std::ostringstream data;
    
    data.precision(fmt);
    data << val;

    return data.str();
}
 
template <typename T> 
std::string Direct<T, when_floating_point(T)>::signature()
    const noexcept {
    return type_of<T>();
}
        
template <typename T> 
T Direct<T, when_pair(T)>::from_str(std::list<std::string> &tokens,
                                    Error &error) const noexcept {

    /* Only considering basic types internally */
    if ( (tokens.size() != 5) || (tokens.front() != "(") || 
         (tokens.back() != ")") ) {
        error = Error::INVALID_VALUE;
        return T();
    }

    tokens.pop_front();
    tokens.pop_back();
    Error error_key(Error::NONE);
    Error error_content(Error::NONE);

    T value(key.from_str(tokens.front(), error_key),
            content.from_str(tokens.back(), error_content));

    tokens.pop_front();

    if ( (error_key != Error::NONE) || (error_content != Error::NONE) || 
         (tokens.front() != ",") ) {
        error = Error::INVALID_VALUE;
    }

    return std::move(value);
}

template <typename T> 
T Direct<T, when_pair(T)>::from_str(std::string &/*token*/, Error &error) 
    const noexcept {
    error = too_few_tokens();
    return T();
}

template <typename T> 
std::string Direct<T, when_pair(T)>::to_str(const T &val) const noexcept {
    return "(" + key.to_str(val.first) + ", " + content.to_str(val.second) +
           ")";
}

template <typename T> 
std::string Direct<T, when_pair(T)>::signature() const noexcept {
    return "(" + key.signature() + ", " + content.signature() + ")";
}

template <typename T> 
T Direct<T, when_vector(T)>::from_str(std::list<std::string> &tokens,
                                      Error &error) const noexcept {

    /* Only considering basic types internally */
    if ( (tokens.front() != "(") || (tokens.back() != ")") ) {
        error = Error::INVALID_VALUE;
        return T();
    }

    /* Remove the first and last tokens */
    tokens.pop_front();
    tokens.pop_back();
    T vector;

    for (auto &token : tokens) {
        vector.emplace_back(std::move(Direct<Type>::from_str(token, error)));
        if (error != Error::NONE) {
            break;
        }
    }

    return std::move(vector);
}

/* C++11 does not have an emplace_back for vector<bool> */
template <> 
std::vector<bool> 
Direct<std::vector<bool>>::from_str(std::list<std::string> &tokens,
                                    Error &error) const noexcept {

    /* Only considering basic types internally */
    if ( (tokens.front() != "(") || (tokens.back() != ")") ) {
        error = Error::INVALID_VALUE;
        return std::vector<bool>();
    }

    /* Remove the first and last tokens */
    tokens.pop_front();
    tokens.pop_back();
    std::vector<bool> vector;

    for (auto &token : tokens) {
        vector.push_back(Direct<Type>::from_str(token, error));
        if (error != Error::NONE) {
            break;
        }
    }

    return vector;
}


template <typename T> 
T Direct<T, when_vector(T)>::from_str(std::string &/*token*/, Error &error) 
    const noexcept {
    error = too_few_tokens();
    return T();
}

template <typename T> 
std::string Direct<T, when_vector(T)>::to_str(const T &cont) const noexcept {

    std::string values("(");
    bool first = true;
    for (auto const &value : cont) {
        if (!first) {
            values += ' ';
        }
        values += Direct<Type>::to_str(value);
        first = false;
    }
    values += ')';

    return values;
}

template <typename T> 
std::string Direct<T, when_vector(T)>::to_str(const Type &val) const noexcept {
        return Direct<Type>::to_str(val);
}

template <typename T> 
std::string Direct<T, when_vector(T)>::signature() const noexcept {
    return type_of<T>();
}

template <typename T> 
T Direct<T, 
         when_associative_container(T)>::from_str(std::list<std::string> 
                                                  &tokens,
                                                  Error &error) const noexcept {

    /* Only considering basic types internally */
    if ( (tokens.front() != "{") || (tokens.back() != "}") ) {
        error = Error::INVALID_VALUE;
        return T();
    }

    /* Remove the first and last tokens */
    tokens.pop_front();
    tokens.pop_back();
    T cont;

    while (tokens.size() > 0) {
        if (type::is_pair<Type>::value) {
            if (tokens.size() < 5) {
                error = Error::INVALID_VALUE;
                break;
            }
            std::list<std::string> the_pair;
            auto last = tokens.begin();
            std::advance(last, 5);
            the_pair.splice(the_pair.begin(), tokens, tokens.begin(), last);
            cont.emplace(std::move(Direct<Type>::from_str(the_pair, error)));
        } else {
            cont.emplace(std::move(Direct<Type>::from_str(tokens.front(),
                         error)));
            tokens.pop_front();
        }

        if (tokens.size() > 0) {
            if (tokens.front() != ",") {
                error = Error::INVALID_VALUE;
            }
            tokens.pop_front();
        }

        if (error != Error::NONE) {
            break;
        }
    }

    return std::move(cont);
}

template <typename T> 
T Direct<T, when_associative_container(T)>::from_str(std::string &/*token*/, 
                                                     Error &error) 
    const noexcept {
    error = too_few_tokens();
    return T();
}

template <typename T> 
std::string Direct<T, when_associative_container(T)>::to_str(const T &cont) 
    const noexcept {

    std::string values("{ ");
    bool first = true;
    for (auto const &value : cont) {
        if (!first) {
            values += ", ";
        }
        values += Direct<Type>::to_str(value);
        first = false;
    }
    values += " }";

    return values;
}

template <typename T> 
std::string Direct<T, when_associative_container(T)>::to_str(const Type &val) 
    const noexcept {
        return Direct<Type>::to_str(val);
}

template <typename T> 
std::string Direct<T, when_associative_container(T)>::signature() 
    const noexcept {
    return type_of<T>();
}

template <typename T> 
Mapped<T, when_not_container(T)>::Mapped() noexcept : translator({}) {}

template <typename T> 
void Mapped<T, when_not_container(T)>::define(std::string key, 
                                              Type value) noexcept {
    auto it = translator.find(key);
    if (it != translator.end()) {
        LOGD("A key named '%s' already existed!", key.c_str());
        translator.erase(it);
        translator.emplace_hint(it, std::move(key), std::move(value));
    } else {
        translator.emplace(std::move(key), std::move(value));
    }
}

template <typename T> 
void Mapped<T, when_not_container(T)>::define(std::map<std::string, 
                                                       Type> other) noexcept {
    for (auto &content : other) {
        define(std::move(content.first), std::move(content.second));
    }

}

template <typename T> 
void Mapped<T, when_not_container(T)>::undefine(const std::string &key)
    noexcept {
    LOGD_IF_NOT(translator.find(key) != translator.end(),
                "No key named '%s' exists!", key.c_str());
    translator.erase(key);
}

template <typename T> 
void Mapped<T,
            when_not_container(T)>::undefine(const std::set<std::string> &keys)
    noexcept {
    for (auto const &key : keys) {
        undefine(key);
    }
}

template <typename T> 
T Mapped<T, when_not_container(T)>::from_str(std::list<std::string> &tokens,
                                             Error &error) const noexcept {
    error = too_many_tokens_in(tokens);
    return T();
}

template <typename T> 
T Mapped<T, when_not_container(T)>::from_str(std::string &key,
                                             Error &error) const noexcept {
    auto it = translator.find(key);
    if (it != translator.end()) {
        return it->second;
    } else {
        error = Error::INVALID_VALUE;
        return T();
    }
}

template <typename T> 
std::string Mapped<T, when_not_container(T)>::to_str(const T &val) 
    const noexcept {
    for (auto const &pair : translator) {
        if (pair.second == val) {
            return pair.first;
        }
    }

    return "????";
}

template <typename T> 
std::string Mapped<T, when_not_container(T)>::signature()
    const noexcept {
    std::string values("string id");
    append_valid_values(values);
    return values;
}

template <typename T> 
void Mapped<T, when_not_container(T)>::append_valid_values(std::string &values)
    const noexcept {
    values += " in { ";
    bool first = true;
    for (auto const &pair : translator) {
        if (!first) {
            values += ", ";
        }
        values += pair.first;
        first = false;
    }
    values += " }";
}

template <typename T> 
Mapped<T, when_vector(T)>::Mapped() noexcept : Mapped<Type>() {}

template <typename T> 
T Mapped<T, when_vector(T)>::from_str(std::list<std::string> &tokens,
                                      Error &error) const noexcept {
    /* Only considering basic types internally */
    if ( (tokens.front() != "(") || (tokens.back() != ")") ) {
        error = Error::INVALID_VALUE;
        return T();
    }

    /* Remove the first and last tokens */
    tokens.pop_front();
    tokens.pop_back();
    T vector;

    for (auto &token : tokens) {
        vector.emplace_back(std::move(Mapped<Type>::from_str(token, error)));
        if (error != Error::NONE) {
            break;
        }
    }

    return std::move(vector);
}

/* C++11 does not have an emplace_back for vector<bool> */
template <> 
std::vector<bool> 
Mapped<std::vector<bool>>::from_str(std::list<std::string> &tokens,
                                    Error &error) const noexcept {
    /* Only considering basic types internally */
    if ( (tokens.front() != "(") || (tokens.back() != ")") ) {
        error = Error::INVALID_VALUE;
        return std::vector<bool>();
    }

    /* Remove the first and last tokens */
    tokens.pop_front();
    tokens.pop_back();
    std::vector<bool> vector;

    for (auto &token : tokens) {
        vector.push_back(Mapped<Type>::from_str(token, error));
        if (error != Error::NONE) {
            break;
        }
    }

    return vector;
}

template <typename T> 
T Mapped<T, when_vector(T)>::from_str(std::string &/*key*/,
                                      Error &error) const noexcept {
    error = too_few_tokens();
    return T();
}

template <typename T> 
std::string Mapped<T, when_vector(T)>::to_str(const T &cont) const noexcept {

    std::string values("(");
    bool first = true;
    for (auto const &value : cont) {
        if (!first) {
            values += ' ';
        }
        values += Mapped<Type>::to_str(value);
        first = false;
    }
    values += ')';

    return values;
}

template <typename T> 
std::string Mapped<T, when_vector(T)>::to_str(const Type &val) const noexcept {
        return Mapped<Type>::to_str(val);
}

template <typename T> 
std::string Mapped<T, when_vector(T)>::signature() const noexcept {
        
    std::string values("(string id ...)");
    Mapped<Type>::append_valid_values(values);
    return values;
}

template <typename T> 
Mapped<T, when_set(T)>::Mapped() noexcept : Mapped<Type>() {}

template <typename T> 
T Mapped<T, when_set(T)>::from_str(std::list<std::string> &tokens,
                                   Error &error) const noexcept {
    /* Only considering basic types internally */
    if ( (tokens.front() != "{") || (tokens.back() != "}") ) {
        error = Error::INVALID_VALUE;
        return T();
    }

    /* Remove the first and last tokens */
    tokens.pop_front();
    tokens.pop_back();
    T set;
    while (tokens.size() > 0) {
        set.emplace(std::move(Mapped<Type>::from_str(tokens.front(), error)));
        tokens.pop_front();

        if (tokens.size() > 0) {
            if (tokens.front() != ",") {
                error = Error::INVALID_VALUE;
            }
            tokens.pop_front();
        }

        if (error != Error::NONE) {
            break;
        }
    }

    return std::move(set);
}

template <typename T> 
T Mapped<T, when_set(T)>::from_str(std::string &/*key*/,
                                      Error &error) const noexcept {
    error = too_few_tokens();
    return T();
}

template <typename T> 
std::string Mapped<T, when_set(T)>::to_str(const T &cont) const noexcept {

    std::string values("{ ");
    bool first = true;
    for (auto const &value : cont) {
        if (!first) {
            values += ", ";
        }
        values += Mapped<Type>::to_str(value);
        first = false;
    }
    values += " }";

    return values;
}

template <typename T> 
std::string Mapped<T, when_set(T)>::to_str(const Type &val) const noexcept {
        return Mapped<Type>::to_str(val);
}

template <typename T> 
std::string Mapped<T, when_set(T)>::signature() const noexcept {
        
    std::string values("{ string id, ... }");
    Mapped<Type>::append_valid_values(values);
    return values;
}

}  // namespace Translator
}  // namespace Customisation
