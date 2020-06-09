/**
 *
 * @file      tst/entity.cpp
 *
 * @brief     This is a set of unit tests for Customisation entities
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
#include "customisation/parameter.hpp"
#include <iostream>

class Engine : public Parametrisable {
    public:
        Engine() : Customisation::Entity("Engine") {
            params.denominate("parameters");
            params.describe("Parameters for the engine");
            expose(params).characterise(Customisation::Trait::CONFIGURABLE);
        }

        void use(std::set<int32_t> p) {
            params = p;
        }

    private:
        PARAMETER(Direct, None, Immediate, std::set<int32_t>) params;
};

class Stage : public Parametrisable {
    public:
        Stage() : Customisation::Entity("Stage") {
            for (int i=0; i < 8; i++) {
                auto name = std::string("engine")+std::to_string(i+1);
                engine[i].denominate(name);
                engine[i].use( { i, i+8, i+16, i+42 });
                current.allow(name);
                expose(engine[i]);
            }

            asynchronous.denominate("asynchronous");
            asynchronous.describe("Using asynchronous engines ?");
            asynchronous = false;
            asynchronous.use(Customisation::Translator::BoolFormat::NO_YES);
            expose(asynchronous).characterise(Customisation::Trait::SETTABLE);

            current.denominate("engine");
            current.describe("Name of the current engine");
            current = "engine1";
            expose(current).characterise(Customisation::Trait::SETTABLE);
        }

    private:
        Engine engine[8];
        PARAMETER(Direct, None, Immediate, bool) asynchronous;
        PARAMETER(Direct, WhiteListed, Immediate, std::string) current;
};

class TopLevel : public Parametrisable {
    public:
        TopLevel() : Customisation::Entity("TopLevel"), path("/home") {
            path.denominate("path").describe("Path to the top-level config");
            expose(path).characterise(Customisation::Trait::CONFIGURABLE);

            ratio.denominate("ratio").describe("Percentage of coverage");
            ratio.range(0, 100);
            expose(ratio).characterise(Customisation::Trait::SETTABLE);

            compute.denominate("compute");
            expose(compute);
        }

    private:
        PARAMETER(Direct, Saturating, Immediate, int)   ratio;
        PARAMETER(Direct, None, Immediate, std::string) path;
        Stage compute;
};

int main(void) {
    TopLevel toplevel;

    toplevel.set("compute.asynchronous", "yes");

    std::cout << std::endl << "*** Displaying snapshot" << std::endl;
    for (auto &l : toplevel.snapshot()) {
        std::cout << l << std::endl;
    }

   std::cout << std::endl << "*** Displaying configured" << std::endl;
    for (auto &l : toplevel.configured()) {
        std::cout << l << std::endl;
    }

    std::cout << std::endl << "*** Displaying configurables" << std::endl;
    for (auto &l : toplevel.configurables()) {
        std::cout << l << std::endl;
    }

    std::cout << std::endl << "*** Displaying setters" << std::endl;
    for (auto &l : toplevel.setters()) {
        std::cout << l << std::endl;
    }

    std::cout << std::endl << "*** Displaying getters" << std::endl;
    for (auto &l : toplevel.getters()) {
        std::cout << l << std::endl;
    }

    std::cout << std::endl << "*** Displaying structure" << std::endl;
    for (auto &l : toplevel.structure()) {
        std::cout << l << std::endl;
    }

    return 0;
}
