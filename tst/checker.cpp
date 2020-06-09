/**
 *
 * @file      tst/checker.cpp
 *
 * @brief     This is a set of unit tests for Customisation parameter checkers
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

template <typename T> static void test(const std::string &kind) {
    T input =  { -10, -8, -6, -4, -2, 0, 2, 4, 6, 8, 10 }; 
    std::set<int> list = { -10, -6, -2, 2, 6, 10 };

    PARAMETER(Direct, None, Immediate, T) unchanged;
    PARAMETER(Direct, WhiteListed, Immediate, T) whitelist;
    PARAMETER(Direct, BlackListed, Immediate, T) blacklist;
    PARAMETER(Direct, Bounded, Immediate, T) bounded;
    PARAMETER(Direct, Saturating, Immediate, T) saturating;

    unchanged.denominate("Unchanged");
    unchanged.characterise(Customisation::Trait::SETTABLE);
    whitelist.denominate("WhiteList");
    whitelist.characterise(Customisation::Trait::SETTABLE);
    whitelist.allow(list);
    blacklist.denominate("BlackList");
    blacklist.characterise(Customisation::Trait::SETTABLE);
    blacklist.forbid(list);
    bounded.denominate("Bounded");
    bounded.characterise(Customisation::Trait::SETTABLE);
    bounded.range(-5, 5);
    saturating.denominate("Saturating");
    saturating.characterise(Customisation::Trait::SETTABLE);
    saturating.range(-5, 5);
    
    unchanged = input;
    whitelist = input;
    blacklist = input;
    bounded = input;
    saturating = input;

    std::cout << std::endl << "Setting all parameters with the following "
              << kind << ": " << std::endl << unchanged.values() << " = "
              << unchanged.value() << std::endl << std::endl;

    std::cout << std::endl << "The " << whitelist.name() << " " << kind
              << " parameter is defined as follows:" << std::endl 
              << "\t" << whitelist.values() << std::endl;

    std::cout << std::endl << "The " << blacklist.name() << " " << kind
              << " parameter is defined as follows:" << std::endl 
              << "\t" << blacklist.values() << std::endl;

    std::cout << std::endl << "The " << bounded.name() << " " << kind
              << " parameter is defined as follows:" << std::endl 
              << "\t" << bounded.values() << std::endl;

    std::cout << std::endl << "The " << saturating.name() << " " << kind
              << " parameter is defined as follows:" << std::endl 
              << "\t" << saturating.values() << std::endl;

    std::cout << std::endl << "And here are the results:"
              << std::endl;

    std::cout << std::endl << "The " << whitelist.name()
              << " parameter contains the following " << kind << ":" 
              << std::endl << "\t" << whitelist.value() << std::endl;

    std::cout << std::endl << "The " << blacklist.name()
              << " parameter contains the following " << kind << ":" 
              << std::endl << "\t" << blacklist.value() << std::endl;

    std::cout << std::endl << "The " << bounded.name()
              << " parameter contains the following " << kind << ":" 
              << std::endl << "\t" << bounded.value() << std::endl;

    std::cout << std::endl << "The " << saturating.name()
              << " parameter contains the following " << kind << ":" 
              << std::endl << "\t" << saturating.value() << std::endl;
}

/* Entry for the test */
int main(void) {
    
    test<std::vector<int>>("vector");
    test<std::set<int>>("set");

    return 0;
}
