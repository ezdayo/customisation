/**
 *
 * @file      src/checker.cpp
 *
 * @brief     This is the implementation of all kinds of parameter checkers
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

#include "checker.tpl.hpp"

/* Checker::None applies to everything */
#define IMPLEMENT_CHECKER_NONE(T) \
template class Customisation::Checker::None<T>; \
template class Customisation::Checker::None<std::set<T>>; \
template class Customisation::Checker::None<std::vector<T>>;

/* Checker listed apply to everything except pointers! */
#define IMPLEMENT_CHECKER_LISTED(T) \
template class Customisation::Checker::WhiteListed<T>; \
template class Customisation::Checker::WhiteListed<std::set<T>>; \
template class Customisation::Checker::WhiteListed<std::vector<T>>; \
template class Customisation::Checker::BlackListed<T>; \
template class Customisation::Checker::BlackListed<std::set<T>>; \
template class Customisation::Checker::BlackListed<std::vector<T>>;

/* Checker ranged apply to arithmetic and arithmetic containers */
#define IMPLEMENT_CHECKER_RANGED(T) \
template class Customisation::Checker::Bounded<T>; \
template class Customisation::Checker::Bounded<std::set<T>>; \
template class Customisation::Checker::Bounded<std::vector<T>>; \
template class Customisation::Checker::Saturating<T>; \
template class Customisation::Checker::Saturating<std::set<T>>; \
template class Customisation::Checker::Saturating<std::vector<T>>;

#define IMPLEMENT_CHECKERS_STRING(T) \
IMPLEMENT_CHECKER_NONE(T) \
IMPLEMENT_CHECKER_LISTED(T)

#define IMPLEMENT_CHECKERS_ARITH(T) \
IMPLEMENT_CHECKER_NONE(T) \
IMPLEMENT_CHECKER_LISTED(T) \
IMPLEMENT_CHECKER_RANGED(T) \

#define IMPLEMENT_CHECKERS_PAIR_MAP_KEY_VAL(T1,T2) \
template class Customisation::Checker::None<std::pair<T1, T2>>; \
template class Customisation::Checker::None<std::map<T1, T2>> ; \
template class Customisation::Checker::WhiteListed<std::pair<T1, T2>>; \
template class Customisation::Checker::WhiteListed<std::map<T1, T2>>; \
template class Customisation::Checker::BlackListed<std::pair<T1, T2>>; \
template class Customisation::Checker::BlackListed<std::map<T1, T2>>;

#define IMPLEMENT_CHECKERS_PAIR_MAP_KEY(T1) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY_VAL(T1, bool) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY_VAL(T1, int8_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY_VAL(T1, int16_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY_VAL(T1, int32_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY_VAL(T1, int64_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY_VAL(T1, uint8_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY_VAL(T1, uint16_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY_VAL(T1, uint32_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY_VAL(T1, uint64_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY_VAL(T1, float) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY_VAL(T1, double) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY_VAL(T1, std::string)

#define IMPLEMENT_CHECKERS \
IMPLEMENT_CHECKERS_STRING(std::string) \
IMPLEMENT_CHECKER_NONE(bool) \
IMPLEMENT_CHECKERS_ARITH(int8_t) \
IMPLEMENT_CHECKERS_ARITH(int16_t) \
IMPLEMENT_CHECKERS_ARITH(int32_t) \
IMPLEMENT_CHECKERS_ARITH(int64_t) \
IMPLEMENT_CHECKERS_ARITH(uint8_t) \
IMPLEMENT_CHECKERS_ARITH(uint16_t) \
IMPLEMENT_CHECKERS_ARITH(uint32_t) \
IMPLEMENT_CHECKERS_ARITH(uint64_t) \
IMPLEMENT_CHECKERS_ARITH(float) \
IMPLEMENT_CHECKERS_ARITH(double) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(int8_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(int16_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(int32_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(int64_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(uint8_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(uint16_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(uint32_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(uint64_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(std::string) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(const int8_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(const int16_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(const int32_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(const int64_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(const uint8_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(const uint16_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(const uint32_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(const uint64_t) \
IMPLEMENT_CHECKERS_PAIR_MAP_KEY(const std::string) \
IMPLEMENT_CHECKER_NONE(Customisation::Entity*) \
IMPLEMENT_CHECKER_NONE(Customisation::Component*) \
IMPLEMENT_CHECKER_NONE(void*) \
IMPLEMENT_CHECKER_NONE(void (*)())

/*
 * Implement'em all!!!
 */
IMPLEMENT_CHECKERS
