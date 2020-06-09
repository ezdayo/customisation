/**
 *
 * @file      tst/storage.cpp
 *
 * @brief     This is a set of unit tests for Customisation parameter storages
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

/* This is the callback function for the callable parameter */
Customisation::Error callback(const std::string &val) {
    std::cout << "*** callback() called with '" << val << "'!"
              << std::endl << std::endl;

    return Customisation::Error::NONE;
}

/* Entry for the test */
int main(void) {

    std::cout << std::endl << "Setting all parameters with their names:"
              << std::endl << std::endl;

    PARAMETER(Direct, None, Immediate, std::string) immediate("immediate");
    PARAMETER(Direct, None, Callable,  std::string) callable("callable");
    PARAMETER(Direct, None, Deferred,  std::string) deferred("deferred");
    PARAMETER(Direct, None, ReadOnly,  std::string) readonly("readonly");

    immediate.denominate("IMM").describe("An immediately modifiable parameter");
    callable.denominate("CAL").describe("A modification-callable parameter");
    deferred.denominate("DEF").describe("A deferred modification parameter");
    readonly.denominate("RO").describe("A read-only parameter");
   
    /* In this simple case callable.trigger(callback) works too! */
    callable.trigger([](const std::string &val) { return callback(val); });
    
    std::cout << "Parameter " << immediate.name() << " initialised with '"
              << (std::string) immediate << "'." << std::endl;
    std::cout << "Parameter " << callable.name() << " initialised with '"
              << (std::string) callable << "'." << std::endl;
    std::cout << "Parameter " << deferred.name() << " initialised with '"
              << (std::string) deferred << "'." << std::endl;
    std::cout << "Parameter " << readonly.name() << " initialised with '"
              << (std::string) readonly << "'." << std::endl;

    std::cout << std::endl << "Changing them all to 'changed':"
              << std::endl << std::endl;
    immediate = "changed";
    callable = "changed";
    deferred = "changed";
    readonly = "changed";

    std::cout << "Parameter " << immediate.name() << " changed to '"
              << (std::string) immediate << "'." << std::endl;
    std::cout << "Parameter " << callable.name() << " changed to '"
              << (std::string) callable << "'." << std::endl;
    std::cout << "Parameter " << deferred.name() << " changed to '"
              << (std::string) deferred << "'." << std::endl;
    std::cout << "Parameter " << readonly.name() << " changed to '"
              << (std::string) readonly << "'." << std::endl << std::endl;

    std::cout << std::endl << "Updating deferred internal content:"
              << std::endl << std::endl;
    deferred.update();

    std::cout << "Parameter " << immediate.name() << " updated to '"
              << (std::string) immediate << "'." << std::endl;
    std::cout << "Parameter " << callable.name() << " updated to '"
              << (std::string) callable << "'." << std::endl;
    std::cout << "Parameter " << deferred.name() << " updated to '"
              << (std::string) deferred << "'." << std::endl;
    std::cout << "Parameter " << readonly.name() << " updated to '"
              << (std::string) readonly << "'." << std::endl << std::endl;

    return 0;
}
