/**
 *
 * @file      src/types.cpp
 *
 * @brief     This is the implementation of a single helper function aimed at
 *            providing a string description of any Customisation component type
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

#include "customisation/entity.hpp"
#include "types.hpp"
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

/* Implement the type_of specialisation for Components and Entities */

#define IMPLEMENT_TYPEOF(t, v) \
template <> const std::string &type_of<t>() noexcept { \
    static const std::string the_type(""#v); \
    return the_type; \
}

IMPLEMENT_TYPEOF(Customisation::Entity, Entity)
IMPLEMENT_TYPEOF(Customisation::Entity&, Entity&)
IMPLEMENT_TYPEOF(Customisation::Entity*, Entity*)
IMPLEMENT_TYPEOF(Customisation::Component, Component)
IMPLEMENT_TYPEOF(Customisation::Component&, Component&)
IMPLEMENT_TYPEOF(Customisation::Component*, Component*)
IMPLEMENT_TYPEOF(void*, void*)
IMPLEMENT_TYPEOF(void(*)(), void(*)())

/* Implement the type_of specialisation for basic types, vectors of basic types,
 * and set of basic types */

#define IMPLEMENT_TYPEOF_CONTAINER(t, v) \
template <> const std::string &type_of<std::vector<t>>() noexcept { \
    static const std::string the_type("("#v" ...)"); \
    return the_type; \
} \
template <> const std::string &type_of<std::set<t>>() noexcept { \
    static const std::string the_type("{ "#v", ... }"); \
    return the_type; \
}

#define IMPLEMENT_ALL_TYPEOF(t, v) \
IMPLEMENT_TYPEOF(t, v) \
IMPLEMENT_TYPEOF(const t, v) \
IMPLEMENT_TYPEOF_CONTAINER(t, v) \
IMPLEMENT_TYPEOF_CONTAINER(const t, v)

IMPLEMENT_ALL_TYPEOF(bool, bool)
IMPLEMENT_ALL_TYPEOF(int8_t, int8)
IMPLEMENT_ALL_TYPEOF(int16_t, int16)
IMPLEMENT_ALL_TYPEOF(int32_t, int32)
IMPLEMENT_ALL_TYPEOF(int64_t, int64)
IMPLEMENT_ALL_TYPEOF(uint8_t, uint8)
IMPLEMENT_ALL_TYPEOF(uint16_t, uint16)
IMPLEMENT_ALL_TYPEOF(uint32_t, uint32)
IMPLEMENT_ALL_TYPEOF(uint64_t, uint64)
IMPLEMENT_ALL_TYPEOF(float, float32)
IMPLEMENT_ALL_TYPEOF(double, float64)
IMPLEMENT_ALL_TYPEOF(std::string, string)

/* Implement the type_of specialisation for maps of basic types */

#define IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER(k, t, ks, ts) \
template <> const std::string &type_of<std::pair<k, t>>() noexcept { \
    static const std::string the_type("("#ks", "#ts")"); \
    return the_type; \
} \
template <> const std::string &type_of<std::pair<const k, t>>() noexcept { \
    static const std::string the_type("("#ks", "#ts")"); \
    return the_type; \
} \
template <> const std::string &type_of<std::map<k, t>>() noexcept { \
    static const std::string the_type("{ ("#ks", "#ts"), ... }"); \
    return the_type; \
}

#define IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER_KEYED(t, ts) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER(int8_t, t, int8, ts) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER(int16_t, t, int16, ts) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER(int32_t, t, int32, ts) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER(int64_t, t, int64, ts) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER(uint8_t, t, uint8, ts) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER(uint16_t, t, uint16, ts) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER(uint32_t, t, uint32, ts) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER(uint64_t, t, uint64, ts) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER(std::string, t, string, ts)

#define IMPLEMENT_ALL_TYPEOF_ASSOCIATIVE_CONTAINER() \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER_KEYED(bool, bool) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER_KEYED(int8_t, int8) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER_KEYED(int16_t, int16) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER_KEYED(int32_t, int32) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER_KEYED(int64_t, int64) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER_KEYED(uint8_t, uint8) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER_KEYED(uint16_t, uint16) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER_KEYED(uint32_t, uint32) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER_KEYED(uint64_t, uint64) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER_KEYED(float, float32) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER_KEYED(double, float64) \
IMPLEMENT_TYPEOF_ASSOCIATIVE_CONTAINER_KEYED(std::string, string)

IMPLEMENT_ALL_TYPEOF_ASSOCIATIVE_CONTAINER()

