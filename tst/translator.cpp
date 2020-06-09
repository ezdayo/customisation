/**
 *
 * @file      tst/translator.cpp
 *
 * @brief     This is a set of unit tests for Customisation parameter
 *            translators
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

#include "customisation/parameter.hpp"
#include <iostream>
#include <cmath>
#include <set>
#include <vector>

template <typename P> void test_translator(P &param, const char *name, 
                                           const char *desc,
                                           typename P::Type changed) {
    param.denominate(name).describe(desc);
    param.characterise(Customisation::Trait::SETTABLE);
    
    std::cout << param.description() << " parameter named " << param.name() 
              << ":" << std::endl << "\tinitialised with " << param.value();

    param = changed;

    std::cout << std::endl << "\t  and changed to " << param.value() << "."
              << std::endl << std::endl;;
}

/* Entry for the test */
int main(void) {

    std::cout << std::endl;
    using MapOfStringToInt = std::map<std::string,int>;

    PARAMETER(None, None, Immediate, std::string) none("none");
    PARAMETER(Direct, None, Immediate, std::string) direct("direct");
    PARAMETER(Direct, None, Immediate, bool) zero_one(true);
    PARAMETER(Direct, None, Immediate, bool) false_true(true);
    PARAMETER(Direct, None, Immediate, bool) no_yes(true);
    PARAMETER(Direct, None, Immediate, bool) off_on(true);
    PARAMETER(Direct, None, Immediate, int32_t) dec(42);
    PARAMETER(Direct, None, Immediate, int32_t) hex(42);
    PARAMETER(Direct, None, Immediate, int32_t) oct(42);
    PARAMETER(Direct, None, Immediate, double) fp(M_PI);
    PARAMETER(Direct, None, Immediate, double) fp5(M_PI);
    PARAMETER(Direct, None, Immediate, std::vector<int>) vect({ 1, 2, 3 });
    PARAMETER(Direct, None, Immediate, std::set<int>) set({ 1, 2, 3 });
    PARAMETER(Direct, None, Immediate, MapOfStringToInt) 
        map({ {"ONE", 1}, { "TWO", 2}, { "THREE", 3 } });
    PARAMETER(Mapped, None, Immediate, int32_t) str2int(42);
    PARAMETER(Mapped, None, Immediate, std::vector<int>) str2vec({ 1, 2, 3 });
    PARAMETER(Mapped, None, Immediate, std::set<int>) str2set({ 1, 2, 3 });
    
    zero_one.use(Customisation::Translator::BoolFormat::ZERO_ONE);
    false_true.use(Customisation::Translator::BoolFormat::FALSE_TRUE);
    no_yes.use(Customisation::Translator::BoolFormat::NO_YES);
    off_on.use(Customisation::Translator::BoolFormat::OFF_ON);

    dec.use(Customisation::Translator::NumberFormat::DECIMAL);
    hex.use(Customisation::Translator::NumberFormat::HEXADECIMAL);
    oct.use(Customisation::Translator::NumberFormat::OCTAL);

    fp5.precision(5);
    str2int.define("FOURTY-TWO", 42);
    str2int.define("EIGHTY-FOUR", 84);
    str2vec.define({ {"ONE", 1}, { "TWO", 2}, { "THREE", 3 } });
    str2vec.define({ {"FOUR", 4}, { "FIVE", 5}, { "SIX", 6 } });
    str2set.define({ {"ONE", 1}, { "TWO", 2}, { "THREE", 3 } });
    str2set.define({ {"FOUR", 4}, { "FIVE", 5}, { "SIX", 6 } });

    test_translator(none, "NONE", "A basic string", "changed");
    test_translator(direct, "DIRECT", "A basic string", "changed");
    test_translator(zero_one, "ZERO_ONE", "A 0/1 bool", false);
    test_translator(false_true, "FALSE_TRUE", "A false/true bool", false);
    test_translator(no_yes, "NO_YES", "A no/yes bool", false);
    test_translator(off_on, "OFF_ON", "A off/on bool", false);
    test_translator(dec, "DEC", "A decimal integer", 84);
    test_translator(hex, "HEX", "A hexadecimal integer", 84);
    test_translator(oct, "OCT", "An octal integer", 84);
    test_translator(fp, "FP", "A real in full precision", M_E);
    test_translator(fp5, "FP5", "A real in 5-digit precision", M_E);
    test_translator(vect, "VECT", "A vector of integers", { 4, 5, 6 });
    test_translator(set, "SET", "A set of integers", { 4, 5, 6 });
    test_translator(map, "MAP", "A map of string to integers", 
                   { {"FOUR", 4}, { "FIVE", 5}, { "SIX", 6 } });
    test_translator(str2int, "STR2INT", "A mapped integer", 84);
    test_translator(str2vec, "STR2VEC", "A mapped vector of integers",
                    { 4, 5, 6 });
    test_translator(str2set, "STR2SET", "A mapped set of intergers",
                    { 4, 5, 6 });
  
    std::cout << std::endl;

    return 0;
}
