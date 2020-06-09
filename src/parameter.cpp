/**
 *
 * @file      src/parameter.cpp
 *
 * @brief     This is the implementation of Customisation parameters
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

#include "parameter.tpl.hpp"

/* Set a parameter definition macro to speed up its writing */
#define IMPLEMENT_PARAMETER_TCSt(T, C, S, t) \
template class Customisation::Parameter<Customisation::Translator::T, \
                                        Customisation::Checker::C, \
                                        Customisation::Storage::S, t>;

/* All kinds of storages are valid for all kinds of objects */
#define IMPLEMENT_PARAMETERS_TCt(T, C, t) \
IMPLEMENT_PARAMETER_TCSt(T, C, Immediate, t) \
IMPLEMENT_PARAMETER_TCSt(T, C, Callable, t) \
IMPLEMENT_PARAMETER_TCSt(T, C, Deferred, t) \
IMPLEMENT_PARAMETER_TCSt(T, C, ReadOnly, t)

/* Let's start with the None translator valid for strings only */
#define IMPLEMENT_PARAMETERS_TRANS_NONE \
IMPLEMENT_PARAMETERS_TCt(None, None, std::string) \
IMPLEMENT_PARAMETERS_TCt(None, BlackListed, std::string) \
IMPLEMENT_PARAMETERS_TCt(None, WhiteListed, std::string)

/* Continue with all parameters dealing with strings */
#define IMPLEMENT_PARAMETERS_BOOL_ARITH_STRING_Ct(C, t) \
IMPLEMENT_PARAMETERS_TCt(Direct, C, t) \
IMPLEMENT_PARAMETERS_TCt(Mapped, C, t)

#define IMPLEMENT_PARAMETERS_STRING_t(t) \
IMPLEMENT_PARAMETERS_BOOL_ARITH_STRING_Ct(None, t) \
IMPLEMENT_PARAMETERS_BOOL_ARITH_STRING_Ct(WhiteListed, t) \
IMPLEMENT_PARAMETERS_BOOL_ARITH_STRING_Ct(BlackListed, t)

#define IMPLEMENT_PARAMETERS_STRING \
IMPLEMENT_PARAMETERS_STRING_t(std::string) \
IMPLEMENT_PARAMETERS_STRING_t(std::set<std::string>) \
IMPLEMENT_PARAMETERS_STRING_t(std::vector<std::string>)

/* Continue with all parameters dealing with booleans */
#define IMPLEMENT_PARAMETERS_BOOL \
IMPLEMENT_PARAMETERS_BOOL_ARITH_STRING_Ct(None, bool) \
IMPLEMENT_PARAMETERS_BOOL_ARITH_STRING_Ct(None, std::set<bool>) \
IMPLEMENT_PARAMETERS_BOOL_ARITH_STRING_Ct(None, std::vector<bool>)

/* Continue with all parameters dealing with arithmetic types */
#define IMPLEMENT_PARAMETERS_ARITH_t(t) \
IMPLEMENT_PARAMETERS_BOOL_ARITH_STRING_Ct(None, t) \
IMPLEMENT_PARAMETERS_BOOL_ARITH_STRING_Ct(WhiteListed, t) \
IMPLEMENT_PARAMETERS_BOOL_ARITH_STRING_Ct(BlackListed, t) \
IMPLEMENT_PARAMETERS_BOOL_ARITH_STRING_Ct(Bounded, t) \
IMPLEMENT_PARAMETERS_BOOL_ARITH_STRING_Ct(Saturating, t)

#define IMPLEMENT_PARAMETERS_ARITH(t) \
IMPLEMENT_PARAMETERS_ARITH_t(t) \
IMPLEMENT_PARAMETERS_ARITH_t(std::set<t>) \
IMPLEMENT_PARAMETERS_ARITH_t(std::vector<t>)

/* Set a parameter definition macro to speed up pair and maps writing */
#define IMPLEMENT_PARAMETER_PAIR_MAP_TCSctt(T, C, S, c, t1, t2) \
template class Customisation::Parameter<Customisation::Translator::T, \
                                        Customisation::Checker::C, \
                                        Customisation::Storage::S, \
                                        std::c<t1, t2>>;

/* All kinds of storages with direct translators only are valid for all kinds
 * of pair and map objects */
#define IMPLEMENT_PARAMETER_PAIR_MAP_Cctt(C, c, t1, t2) \
IMPLEMENT_PARAMETER_PAIR_MAP_TCSctt(Direct, C, Immediate, c, t1, t2) \
IMPLEMENT_PARAMETER_PAIR_MAP_TCSctt(Direct, C, Callable, c, t1, t2) \
IMPLEMENT_PARAMETER_PAIR_MAP_TCSctt(Direct, C, Deferred, c, t1, t2) \
IMPLEMENT_PARAMETER_PAIR_MAP_TCSctt(Direct, C, ReadOnly, c, t1, t2)

#define IMPLEMENT_PARAMETERS_PAIR_MAP_ctt(c, t1, t2) \
IMPLEMENT_PARAMETER_PAIR_MAP_Cctt(None, c, t1, t2) \
IMPLEMENT_PARAMETER_PAIR_MAP_Cctt(WhiteListed, c, t1, t2) \
IMPLEMENT_PARAMETER_PAIR_MAP_Cctt(BlackListed, c, t1, t2)

#define IMPLEMENT_PARAMETERS_PAIR_MAP_KEY_VAL(t1, t2) \
IMPLEMENT_PARAMETERS_PAIR_MAP_ctt(pair, t1, t2) \
IMPLEMENT_PARAMETERS_PAIR_MAP_ctt(map, t1, t2)

#define IMPLEMENT_PARAMETERS_PAIR_MAP_KEY(t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY_VAL(t, bool) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY_VAL(t, int8_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY_VAL(t, int16_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY_VAL(t, int32_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY_VAL(t, int64_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY_VAL(t, uint8_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY_VAL(t, uint16_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY_VAL(t, uint32_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY_VAL(t, uint64_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY_VAL(t, float) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY_VAL(t, double) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY_VAL(t, std::string)

/* Pointers to objects can only be mapped without any other check */
#define IMPLEMENT_PARAMETERS_POINTERS(t) \
IMPLEMENT_PARAMETERS_TCt(Mapped, None, t)

/* Implement them all! */
#define IMPLEMENT_PARAMETERS \
IMPLEMENT_PARAMETERS_STRING \
IMPLEMENT_PARAMETERS_BOOL \
IMPLEMENT_PARAMETERS_ARITH(int8_t) \
IMPLEMENT_PARAMETERS_ARITH(int16_t) \
IMPLEMENT_PARAMETERS_ARITH(int32_t) \
IMPLEMENT_PARAMETERS_ARITH(int64_t) \
IMPLEMENT_PARAMETERS_ARITH(uint8_t) \
IMPLEMENT_PARAMETERS_ARITH(uint16_t) \
IMPLEMENT_PARAMETERS_ARITH(uint32_t) \
IMPLEMENT_PARAMETERS_ARITH(uint64_t) \
IMPLEMENT_PARAMETERS_ARITH(float) \
IMPLEMENT_PARAMETERS_ARITH(double) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY(int8_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY(int16_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY(int32_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY(int64_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY(uint8_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY(uint16_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY(uint32_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY(uint64_t) \
IMPLEMENT_PARAMETERS_PAIR_MAP_KEY(std::string) \
IMPLEMENT_PARAMETERS_POINTERS(Customisation::Entity*) \
IMPLEMENT_PARAMETERS_POINTERS(Customisation::Component*) \
IMPLEMENT_PARAMETERS_POINTERS(void*) \
IMPLEMENT_PARAMETERS_TRANS_NONE

/*
 * Implement'em all!!!
 */
IMPLEMENT_PARAMETERS

