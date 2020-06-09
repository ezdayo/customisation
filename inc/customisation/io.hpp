/**
 *
 * @file      customisation/io.hpp
 *
 * @brief     This is the definition of a helper class for Customisation
 *            components serialisation and input-output
 *
 * @details   This file contains the definition of the Customisation components
 *            IO helper class. It is a set of functions to load, save, parse and
 *            compose hierarchies of components.
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

#include "customisation/error.hpp"
#include <list>
#include <string>
#include <utility>
#include <vector>

namespace Customisation {

namespace IO {

std::vector<std::string> loaded(const std::string &name, Error &error) noexcept;

Error save(const std::string &name,
           const std::vector<std::string> &lines) noexcept;

Error parse(const std::string &line, std::list<std::string> &tokens,
            int min, int max) noexcept;

Error parse(const std::string &line, std::list<std::string> &tokens,
            int max) noexcept;

Error parse(const std::string &line, std::list<std::string> &tokens) noexcept;

void compose(std::vector<std::string> &output, std::string path,
             const std::string &value, const std::string &comment, 
             const std::string &description) noexcept;

}  // namespace IO
}  // namespace Customisation
