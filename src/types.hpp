/**
 *
 * @file      src/types.hpp
 *
 * @brief     This is the definition of a single helper function aimed at
 *            providing a string description of any Customisation component type
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

#include <string>

template <typename t> const std::string &type_of() noexcept; 
