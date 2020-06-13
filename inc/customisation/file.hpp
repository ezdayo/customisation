/**
 *
 * @file      customisation/file.hpp
 *
 * @brief     This is a simple file wrapper for a Customisation string parameter
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

#include <fstream>
#include <string>

#include "customisation/parameter.hpp"

namespace Customisation {

class File : public PARAMETER(Direct, None, Immediate, std::string) {
    public:
        File() noexcept = default;
        File(std::string name) noexcept 
            : PARAMETER(Direct, None, Immediate, std::string)(std::move(name)) {
        }

        virtual ~File() noexcept = default;

        File &operator = (std::string val) noexcept {
            PARAMETER(Direct, None, Immediate, std::string)::operator = 
                                                            (std::move(val));
            return *this;
        }

        bool exists() const noexcept {
            std::ifstream ifs(static_cast<std::string>(*this));
            return ifs.is_open();
        }

        bool undefined() const noexcept {
            return static_cast<std::string>(*this).empty();
        }
};

}  // namespace Customisation
