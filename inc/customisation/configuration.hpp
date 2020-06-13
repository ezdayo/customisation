/**
 *
 * @file      customisation/configuration.hpp
 *
 * @brief     This is a simple generic class for downloading configuration files
 *            securely. It downloads a master configuration file that lists all
 *            the required configuration and parameter files, which are then
 *            downloaded and MD5-compared with the content of the master config
 *            file.
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

#include "customisation/entity.hpp"
#include "customisation/parameter.hpp"

namespace Customisation {

class Configuration : public Parametrisable {
    public:
        Configuration() noexcept;

        /** Setups cannot be copied nor moved */
        Configuration(const Configuration& other) = delete;
        Configuration(Configuration&& other) = delete;
        Configuration& operator=(const Configuration& other) = delete;
        Configuration& operator=(Configuration&& other) = delete;
        virtual ~Configuration() noexcept = default;

        PARAMETER(Direct, WhiteListed, Immediate, std::string) protocol;
        PARAMETER(Direct, WhiteListed, Immediate, std::string) server;
        PARAMETER(Direct, WhiteListed, Immediate, std::string) path;
        PARAMETER(Direct, WhiteListed, Immediate, std::string) folder;
        PARAMETER(Direct, Saturating,  Immediate, int32_t) timeout;
        PARAMETER(Direct, WhiteListed, Immediate, int32_t) port;

        /* User and password are provided as external functions so that it can
         * use the secure capabilities of the OS without storing any critical
         * information here */
        std::function<void (std::string &u) noexcept> user;
        std::function<void (std::string &p) noexcept> password;

        Customisation::Error setup() noexcept;
};

}  // namespace Customisation
