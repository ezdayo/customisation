/**
 *
 * @file      src/translator.cpp
 *
 * @brief     This is the implementation of Customisation parameter translator
 *            classes
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

#include "translator.tpl.hpp"

/* Translator::None only applies to strings, nothing else */
#define IMPLEMENT_TRANSLATOR_NONE(T) \
template class Customisation::Translator::None<T>;

/* Translator::Direct applies to strings, bools, arithmetics, pairs, vectors,
 * sets and maps */
#define IMPLEMENT_TRANSLATOR_DIRECT(T) \
template class Customisation::Translator::Direct<T>; \
template class Customisation::Translator::Direct<std::set<T>>; \
template class Customisation::Translator::Direct<std::vector<T>>;

/* Translator::Mapped applies to any non-container type, plus vectors and sets 
 * of those types */
#define IMPLEMENT_TRANSLATOR_MAPPED(T) \
template class Customisation::Translator::Mapped<T>; \
template class Customisation::Translator::Mapped<std::set<T>>; \
template class Customisation::Translator::Mapped<std::vector<T>>;

#define IMPLEMENT_TRANSLATORS_STRING(T) \
IMPLEMENT_TRANSLATOR_NONE(T) \
IMPLEMENT_TRANSLATOR_DIRECT(T) \
IMPLEMENT_TRANSLATOR_MAPPED(T)

#define IMPLEMENT_TRANSLATORS_BOOL_ARITH(T) \
IMPLEMENT_TRANSLATOR_DIRECT(T) \
IMPLEMENT_TRANSLATOR_MAPPED(T)

#define IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY_VAL(T1,T2) \
template class Customisation::Translator::Direct<std::pair<T1, T2>>; \
template class Customisation::Translator::Direct<std::map<T1, T2>>;

#define IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY(T1) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY_VAL(T1, bool) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY_VAL(T1, int8_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY_VAL(T1, int16_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY_VAL(T1, int32_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY_VAL(T1, int64_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY_VAL(T1, uint8_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY_VAL(T1, uint16_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY_VAL(T1, uint32_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY_VAL(T1, uint64_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY_VAL(T1, float) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY_VAL(T1, double) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY_VAL(T1, std::string)

#define IMPLEMENT_TRANSLATORS \
IMPLEMENT_TRANSLATORS_STRING(std::string) \
IMPLEMENT_TRANSLATORS_BOOL_ARITH(bool) \
IMPLEMENT_TRANSLATORS_BOOL_ARITH(int8_t) \
IMPLEMENT_TRANSLATORS_BOOL_ARITH(int16_t) \
IMPLEMENT_TRANSLATORS_BOOL_ARITH(int32_t) \
IMPLEMENT_TRANSLATORS_BOOL_ARITH(int64_t) \
IMPLEMENT_TRANSLATORS_BOOL_ARITH(uint8_t) \
IMPLEMENT_TRANSLATORS_BOOL_ARITH(uint16_t) \
IMPLEMENT_TRANSLATORS_BOOL_ARITH(uint32_t) \
IMPLEMENT_TRANSLATORS_BOOL_ARITH(uint64_t) \
IMPLEMENT_TRANSLATORS_BOOL_ARITH(float) \
IMPLEMENT_TRANSLATORS_BOOL_ARITH(double) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY(int8_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY(int16_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY(int32_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY(int64_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY(uint8_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY(uint16_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY(uint32_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY(uint64_t) \
IMPLEMENT_TRANSLATORS_PAIR_MAP_KEY(std::string) \
IMPLEMENT_TRANSLATOR_MAPPED(Customisation::Entity*) \
IMPLEMENT_TRANSLATOR_MAPPED(Customisation::Component*) \
IMPLEMENT_TRANSLATOR_MAPPED(void*)

/*
 * Implement'em all!!!
 */
IMPLEMENT_TRANSLATORS

