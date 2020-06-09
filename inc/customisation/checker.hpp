/**
 *
 * @file      customisation/checker.hpp
 *
 * @brief     This is the definition of all kinds of parameter checkers
 *
 * @details   This file contains the definition of all checkers that can be
 *            used in Customisation parameters. A checker ensures that the
 *            provided value is a valid one. A checker can be of:
 *            - None if no checking is required;
 *            - Whitelisted for only allowing a finite set of valid values;
 *            - Blacklisted for preventing a finite set of invalid values;
 *            - Bounded for allowing a range of values;
 *            - Saturating for allowing a range of values with saturation.
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
#include <string>
#include <set>

namespace Customisation {
namespace Checker {

enum class Kind : int {
    NONE       = 0,
    RANGE      = 1,
    WHITE_LIST = 2,
    BLACK_LIST = 3
};

template <typename T, typename Default = void>
class None {
    public:
        using Type = T;
        
    protected:
        Type check(Type val, Error &error) const noexcept;
        const std::set<Type> &validity(Kind &kind) const noexcept;

    private:
        static std::set<Type> empty;
};

template <typename T, typename Default = void>
class WhiteListed {
    public:
        using Type = T;
        
        void allow() noexcept;
        void allow(Type value) noexcept;
        void allow(std::set<Type> values) noexcept;
        void forbid(const Type &value) noexcept;
        void forbid(const std::set<Type> &values) noexcept;

    protected:
        Type check(Type value, Error &error) const noexcept;
        const std::set<Type> &validity(Kind &kind) const noexcept;

        std::set<Type> _allowed;

    private:
        static std::set<Type> empty;
};

template <typename T>
class WhiteListed<T, when_container(T)> : 
    public WhiteListed<typename T::value_type> {
    public:
        using Container = T;
        using Type = typename T::value_type;

    protected:
        Container check(Container values, Error &error) const noexcept;
};

template <typename T, typename Default = void>
class BlackListed {
    public:
        using Type = T;
        
        void allow(const Type &value) noexcept;
        void allow(const std::set<Type> &values) noexcept;
        void forbid() noexcept;
        void forbid(Type value) noexcept;
        void forbid(std::set<Type> values) noexcept;

    protected:
        Type check(Type value, Error &error) const noexcept;
        const std::set<Type> &validity(Kind &kind) const noexcept;

        std::set<Type> _forbidden;
};

template <typename T>
class BlackListed<T, when_container(T)> : 
    public BlackListed<typename T::value_type> {
    public:
        using Container = T;
        using Type = typename T::value_type;

    protected:
        Container check(Container values, Error &error) const noexcept;
};

template <typename T, typename Default = void> class Bounded;
template <typename T>
class Bounded<T, when_arithmetic(T)>
  {
    public:
        using Type = T;

        Bounded() noexcept;

        Error range(Type min, Type max) noexcept;
        Type min() const noexcept;
        Type max() const noexcept;

    protected:
        Type check(Type value, Error &error) const noexcept;
        const std::set<Type> &validity(Kind &kind) const noexcept;

        Type           _min;
        Type           _max;
        std::set<Type> _range;
};

template <typename T>
class Bounded<T, when_arithmetic_container(T)> : 
    public Bounded<typename T::value_type> {
    public:
        using Container = T;
        using Type = typename T::value_type;

    protected:
        Container check(Container values, Error &error) const noexcept;
};

template <typename T, typename Default = void> class Saturating;
template <typename T>
class Saturating<T, when_arithmetic(T)> : public Bounded<T>
{
    public:
        using Type = T;

    protected:
        Type check(Type value, Error &error) const noexcept;
};

template <typename T>
class Saturating<T, when_set(T)> : 
    public Saturating<typename T::value_type> {
    public:
        using Container = T;
        using Type = typename T::value_type;

    protected:
        Container check(Container values, Error &error) const noexcept;
};

template <typename T>
class Saturating<T, when_vector(T)> : 
    public Saturating<typename T::value_type> {
    public:
        using Container = T;
        using Type = typename T::value_type;

    protected:
        Container check(Container values, Error &error) const noexcept;
};

}  // namespace Checker
}  // namespace Customisation

