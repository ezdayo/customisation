/**
 *
 * @file      customisation/socket.hpp
 *
 * @brief     This is a simple wrapper for the Curl library IO
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

#include <curl/curl.h>
#include <string>
#include <vector>

#pragma once

namespace Customisation {

class Socket {
    public:
        Socket() noexcept;
        ~Socket() noexcept;
        
        Socket(const Socket& other) = delete;
        Socket(Socket&& other) = delete;
        Socket& operator=(const Socket& other) = delete;
        Socket& operator=(Socket&& other) = delete;

        int open(const std::string &path) noexcept;

        int open(const std::string &protocol, 
                 const std::string &server) noexcept;

        int open(const std::string &protocol, const std::string &server,
                 const std::string &path) noexcept;

        int setup(const std::string &username,
                  const std::string &password) noexcept;
        
        int setup(const std::string &username, const std::string &password,
                  long timeout) noexcept;

        int setup(const std::string &username, const std::string &password,
                  long timeout, long port) noexcept;

        int get(const std::string &file) const noexcept;

        int get(const std::string &folder, 
                const std::string &file) const noexcept;

        int get(std::vector<unsigned char> &stream) const noexcept;

        int get(std::vector<unsigned char> &stream, 
                const std::string &file) const noexcept;

        int put(const std::string &file) const noexcept;

        int put(const std::string &folder,
                const std::string &file) const noexcept;

        int close() noexcept;

    private:
        std::string  target;
        CURL        *curl;
};

}  // namespace Customisation
