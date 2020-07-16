/**
 *
 * @file      customisation/sfinae.hpp
 *
 * @brief     A set of utility routines to implement SFINAE patterns
 *
 * @details   This file defines some metaprogramming functions and structures
 *            for using Substitution Failure Is Not An Error (SFINAE) patterns
 *            during parameter substitution in C++ templates
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

#include <map>
#include <set>
#include <type_traits>
#include <utility>
#include <vector>

namespace type {

/* The has_member templated meta-structure allows to check if a given type has
 * some defined members, so that a "family" signature can be checked against */
template<typename... Ts> struct has_members {};

template<typename T, typename = void>
struct is_container : public std::false_type {};

template<> struct is_container<std::string> : public std::false_type {};
template<> struct is_container<const std::string> : public std::false_type {};

template<typename T>
struct is_container<T,
  typename std::conditional<false,
                            has_members<typename T::value_type,
                                        typename T::size_type,
                                        typename T::allocator_type,
                                        typename T::iterator,
                                        typename T::const_iterator,
                                        decltype(std::declval<T>().size()),
                                        decltype(std::declval<T>().begin()),
                                        decltype(std::declval<T>().end()),
                                        decltype(std::declval<T>().cbegin()),
                                        decltype(std::declval<T>().cend())>,
                            void>::type> : public std::true_type {};

template<typename T, typename = void>
struct is_associative_container : public std::false_type {};

template<typename T>
struct is_associative_container<T,
  typename std::conditional<false,
                            has_members<typename T::value_type,
                                        typename T::key_type,
                                        typename T::size_type,
                                        typename T::allocator_type,
                                        typename T::iterator,
                                        typename T::const_iterator,
                                        decltype(std::declval<T>().size()),
                                        decltype(std::declval<T>().begin()),
                                        decltype(std::declval<T>().end()),
                                        decltype(std::declval<T>().cbegin()),
                                        decltype(std::declval<T>().cend())>,
                            void>::type> : public std::true_type {};

template <typename T, typename = void>
struct is_pair : public std::false_type {};

template<typename T>
struct is_pair<T,
  typename std::conditional<false,
                            has_members<typename T::first_type,
                                        typename T::second_type>,
                            void>::type> : public std::true_type {};

template <typename T>
using is_map = std::is_same<T, std::map<typename T::key_type,
                                        typename T::mapped_type,
                                        typename T::key_compare,
                                        typename T::allocator_type >>;


template <typename T>
using is_set = std::is_same<T, std::set<typename T::key_type,
                                        typename T::key_compare,
                                        typename T::allocator_type >>;


template <typename T>
using is_vector = std::is_same<T, std::vector<typename T::value_type,
                                              typename T::allocator_type>>;

template <typename T, typename = void>
struct is_ordered : public std::false_type {};

template <typename T>
struct is_ordered<T, 
    typename std::enable_if<std::is_convertible<decltype(std::declval<T&>() < 
                                                         std::declval<T&>()), 
                                                         bool>{}>::type> : 
                            public std::true_type {};

template<typename T, typename = void>
struct is_arithmetic : public std::false_type {};

template<> struct is_arithmetic<bool> : public std::false_type {};
template<> struct is_arithmetic<const bool> : public std::false_type {};

template<typename T>
struct is_arithmetic<T, typename
                     std::enable_if<std::is_arithmetic<T>::value>::type> :
                            public std::true_type {};
template<typename T, typename = void>

struct is_integral : public std::false_type {};

template<> struct is_integral<bool> : public std::false_type {};
template<> struct is_integral<const bool> : public std::false_type {};

template<typename T>
struct is_integral<T, typename
                     std::enable_if<std::is_integral<T>::value>::type> :
                            public std::true_type {};

}  // namespace type

/* A handful of helper macros */
#define when_is(T, C) \
    typename std::enable_if<std::is_same<T, C>::value || \
                            std::is_same<T, const C>::value>::type
#define when_convertible(T, C) \
    typename std::enable_if<std::is_convertible<T, C>::value>::type

#define when_arithmetic(T) \
    typename std::enable_if<type::is_arithmetic<T>::value>::type
#define when_integral(T) \
    typename std::enable_if<type::is_integral<T>::value>::type
#define when_floating_point(T) \
    typename std::enable_if<std::is_floating_point<T>::value>::type

#define when_arithmetic_container(T) \
    typename std::enable_if<type::is_container<T>::value &&  \
                   std::is_arithmetic<typename T::value_type>::value>::type
#define when_container(T) \
    typename std::enable_if<type::is_container<T>::value>::type
#define when_not_container(T) \
    typename std::enable_if<!type::is_container<T>::value>::type
#define when_associative_container(T) \
    typename std::enable_if<type::is_associative_container<T>::value>::type
#define when_pair(T) \
    typename std::enable_if<type::is_pair<T>::value>::type
#define when_map(T) \
    typename std::enable_if<type::is_map<T>::value>::type
#define when_set(T) \
    typename std::enable_if<type::is_set<T>::value>::type
#define when_vector(T) \
    typename std::enable_if<type::is_vector<T>::value>::type
#define when_ordered(T) \
    typename std::enable_if<type::is_ordered<T>::value>::type

#define when_string(T) when_is(T, std::string)
