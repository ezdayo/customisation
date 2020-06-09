/**
 *
 * @file      customisation/traits.hpp
 *
 * @brief     This is the definition of Customisation components traits
 *
 * @details   This file defines the Customisation component traits and some
 *            helper functions to ease their management. A component can hence
 *            be stated as:
 *            - CONFIGURABLE if it can only be configured during setup;
 *            - SETTABLE if it can be set and configured anytime;
 *            - LOCKED if it can no longer be changed;
 *            - ENTITY if the component is actually an entity;
 *            - UNDEFINED if it is yet to be defined.
 *
 * @author    Olivier Stoltz-Douchet <ezdayo@gmail.com>
 *
 * @copyright (c) 2019-2020 Olivier Stoltz-Douchet
 * @license   http://opensource.org/licenses/MIT MIT
 * @link      https://github.com/ezdayo/customisation
 *
 **/

#pragma once

#include <cstdint>
#include <string>

namespace Customisation {

/** Customisation traits */
enum class Trait : uint32_t {
    NONE                 = 0x00000000,
    CONFIGURABLE         = 0x00000001,
    SETTABLE             = 0x00000003,
    LOCKED               = 0x10000000,
    ENTITY               = 0x40000000,
    UNDEFINED            = 0x80000000
};

inline Trait operator ~(Trait t) {
    return static_cast<Trait>(~static_cast<uint32_t>(t));
}

inline Trait operator | (Trait t1, Trait t2) {
    return static_cast<Trait>(static_cast<uint32_t>(t1) | 
                              static_cast<uint32_t>(t2));
}

inline Trait &operator |= (Trait &t1, Trait t2) {
    t1 = static_cast<Trait>(static_cast<uint32_t>(t1) | 
                            static_cast<uint32_t>(t2));
    return t1;
}

inline Trait operator & (Trait t1, Trait t2) {
    return static_cast<Trait>(static_cast<uint32_t>(t1) & 
                              static_cast<uint32_t>(t2));
}

inline Trait &operator &= (Trait &t1, Trait t2) {
    t1 = static_cast<Trait>(static_cast<uint32_t>(t1) & 
                            static_cast<uint32_t>(t2));
    return t1;
}

inline bool operator == (Trait t1, Trait t2) {
    return (static_cast<uint32_t>(t1) == static_cast<uint32_t>(t2));
}

inline bool operator != (Trait t1, Trait t2) {
    return (static_cast<uint32_t>(t1) != static_cast<uint32_t>(t2));
}

inline std::string toString(Trait t) {
    if ((t & Trait::UNDEFINED) == Trait::UNDEFINED) {
        return "UNDEFINED";
    }

    std::string s;
    if ((t & Trait::ENTITY) == Trait::ENTITY) {
        s = "ENTITY";
    } else {
        s = "PARAMETER";
    }

    if ((t & Trait::LOCKED) == Trait::LOCKED) {
        s += "|LOCKED";
    }

    if ((t & Trait::SETTABLE) == Trait::SETTABLE) {
        s += "|SETTABLE";
    }

    if ((t & Trait::CONFIGURABLE) == Trait::CONFIGURABLE) {
        s += "|CONFIGURABLE";
    }

    return s;
}

}  // namespace Customisation
