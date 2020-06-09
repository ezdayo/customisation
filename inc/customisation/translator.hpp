/**
 *
 * @file      customisation/translator.hpp
 *
 * @brief     This is the definition of Customisation parameter translator
 *            classes
 *
 * @details   This file contains the definition of a parameter translator class.
 *            A parameter translation can be any of the following:
 *            - None for strings (translated as strings!);
 *            - Direct for a direct translation of string to the value type;
 *            - Mapped for mapping string inputs to predefined values. 
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
#include "customisation/sfinae.hpp"
#include <list>
#include <map>
#include <set>
#include <string>

namespace Customisation {
namespace Translator {

template <typename T, typename Enable = void> class None;
template <typename T> 
class None<T, when_string(T)> {
    public:
        using Type = T;
    
    protected:
        Type from_str(std::list<std::string> &tokens,
                      Error &error) const noexcept;
        Type from_str(std::string &token, Error &error) const noexcept;
        std::string to_str(const Type &val) const noexcept;
        std::string signature() const noexcept;
};

template <typename T, typename Enable = void> class Direct;

template <typename T> 
class Direct<T, when_string(T)> {
    public:
        using Type = T;
    
        template <typename C, typename Enable> friend class Direct;

    protected:
        Type from_str(std::list<std::string> &tokens,
                      Error &error) const noexcept;
        Type from_str(std::string &token, Error &error) const noexcept;
        std::string to_str(const Type &val) const noexcept;
        std::string signature() const noexcept;
};

enum class BoolFormat : unsigned int {
    ZERO_ONE   = 0,
    FALSE_TRUE = 1,
    NO_YES     = 2,
    OFF_ON     = 3
};

template <typename T> 
class Direct<T, when_is(T, bool)> {
    public:
        using Type = T;
  
        Direct() noexcept;
        void use(BoolFormat format) noexcept;

        template <typename C, typename Enable> friend class Direct;

    protected:
        Type from_str(std::list<std::string> &tokens,
                      Error &error) const noexcept;
        Type from_str(std::string &token, Error &error) const noexcept;
        std::string to_str(const Type &val) const noexcept;
        std::string signature() const noexcept;

    private:
        int fmt;
};

enum class NumberFormat : unsigned int {
    DECIMAL     = 0,
    HEXADECIMAL = 1,
    OCTAL       = 2,
};

template <typename T> 
class Direct<T, when_integral(T)> {
    public:
        using Type = T;

        Direct() noexcept;
        void use(NumberFormat format) noexcept;
    
        template <typename C, typename Enable> friend class Direct;

    protected:
        Type from_str(std::list<std::string> &tokens,
                      Error &error) const noexcept;
        Type from_str(std::string &token, Error &error) const noexcept;
        std::string to_str(const Type &val) const noexcept;
        std::string signature() const noexcept;

    private:
        int fmt;
};

template <typename T> 
class Direct<T, when_floating_point(T)> {
    public:
        using Type = T;

        Direct() noexcept;
        void precision(int prec) noexcept;

        template <typename C, typename Enable> friend class Direct;

    protected:
        Type from_str(std::list<std::string> &tokens,
                      Error &error) const noexcept;
        Type from_str(std::string &token, Error &error) const noexcept;
        std::string to_str(const Type &val) const noexcept;
        std::string signature() const noexcept;

    private:
        int fmt;
};

template <typename T> 
class Direct<T, when_pair(T)> {
    public:
        using Type = T;

        Direct<typename std::remove_cv<typename T::first_type>::type>  key;
        Direct<typename std::remove_cv<typename T::second_type>::type> content;

    protected:
        Type from_str(std::list<std::string> &tokens,
                      Error &error) const noexcept;
        Type from_str(std::string &token, Error &error) const noexcept;
        std::string to_str(const Type &val) const noexcept;
        std::string signature() const noexcept;
};

template <typename T> 
class Direct<T, when_vector(T)> : public Direct<typename T::value_type> {
    public:
        using Container = T;
        using Type = typename T::value_type;

    protected:
        Container from_str(std::list<std::string> &tokens,
                           Error &error) const noexcept;
        Container from_str(std::string &token, Error &error) const noexcept;
        std::string to_str(const Container &cont) const noexcept;
        std::string to_str(const Type &val) const noexcept;
        std::string signature() const noexcept;
};

/* For maps and sets */
template <typename T> 
class Direct<T, when_associative_container(T)> : 
    public Direct<typename T::value_type> {
    public:
        using Container = T;
        using Type = typename std::remove_cv<typename T::value_type>::type;

    protected:
        Container from_str(std::list<std::string> &tokens,
                           Error &error) const noexcept;
        Container from_str(std::string &token, Error &error) const noexcept;
        std::string to_str(const Container &cont) const noexcept;
        std::string to_str(const Type &val) const noexcept;
        std::string signature() const noexcept;
};

template <typename T, typename Enable = void> class Mapped;

template <typename T> 
class Mapped<T, when_not_container(T)> {
    public:
        using Type = T;

        Mapped() noexcept;
        void define(std::string key, Type value) noexcept;
        void define(std::map<std::string, Type> other) noexcept;
        void undefine(const std::string &key) noexcept;
        void undefine(const std::set<std::string> &keys) noexcept;

    protected:
        Type from_str(std::list<std::string> &tokens,
                      Error &error) const noexcept;
        Type from_str(std::string &token, Error &error) const noexcept;
        std::string to_str(const Type &val) const noexcept;
        std::string signature() const noexcept;
        
        void append_valid_values(std::string &values) const noexcept;

    private:
        std::map<std::string, T> translator;
};

template <typename T> 
class Mapped<T, when_vector(T)> : public Mapped<typename T::value_type> {
    public:
        using Container = T;
        using Type = typename T::value_type;

        Mapped() noexcept;

    protected:
        Container from_str(std::list<std::string> &tokens,
                           Error &error) const noexcept;
        Container from_str(std::string &token, Error &error) const noexcept;
        std::string to_str(const Container &cont) const noexcept;
        std::string to_str(const Type &val) const noexcept;
        std::string signature() const noexcept;
};

template <typename T> 
class Mapped<T, when_set(T)> : public Mapped<typename T::value_type> {
    public:
        using Container = T;
        using Type = typename T::value_type;

        Mapped() noexcept;

    protected:
        Container from_str(std::list<std::string> &tokens,
                           Error &error) const noexcept;
        Container from_str(std::string &token, Error &error) const noexcept;
        std::string to_str(const Container &cont) const noexcept;
        std::string to_str(const Type &val) const noexcept;
        std::string signature() const noexcept;
};

}  // namespace Translator
}  // namespace Customisation

