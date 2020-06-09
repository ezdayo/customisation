/**
 *
 * @file      customisation/error.hpp
 *
 * @brief     This is the definition of Customisation error codes
 *
 * @author    Olivier Stoltz-Douchet <ezdayo@gmail.com>
 *
 * @copyright (c) 2019-2020 Olivier Stoltz-Douchet
 * @license   http://opensource.org/licenses/MIT MIT
 * @link      https://github.com/ezdayo/customisation
 *
 **/

#pragma once

namespace Customisation {

/** Customisation error codes */
enum class Error : int {
    NONE            =  0,
    TYPE_MISMATCH   = -1,
    INVALID_REQUEST = -2,
    INVALID_VALUE   = -3,
    INVALID_RANGE   = -4,
    NOT_EXISTING    = -5,
    UNDEFINED       = -6,
    UNKNOWN         = -7
};

}  // namespace Customisation
