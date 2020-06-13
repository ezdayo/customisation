/**
 *
 * @file      src/socket.cpp
 *
 * @brief     This is the implementation of Customisation socket IO
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

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <openssl/md5.h>
#include <sstream>

#include "log.hpp"
#include "customisation/socket.hpp"

namespace Customisation {

static size_t curlWriteStream(char *buffer, size_t size, size_t nmemb,
                              void *stream) noexcept {
    auto data = static_cast<std::vector<unsigned char>* >(stream);
    size_t count = size * nmemb;
    data->insert(data->end(), buffer, buffer + count);

    return count;
}

struct curlFile {
    std::string filename;
    FILE *      stream;
};
 
static size_t curlWriteFile(void *buffer, size_t size, size_t nmemb,
                            void *cfile) noexcept {
    auto out = static_cast<struct curlFile *>(cfile);
    
    if(out->stream == nullptr) {
        out->stream = fopen(out->filename.c_str(), "wb");
        if(out->stream == nullptr)
            return -1;
    }
    
    return fwrite(buffer, size, nmemb, out->stream);
}

static size_t curlReadFile(void *buffer, size_t size, size_t nmemb,
                           void *cfile) noexcept {
    struct curlFile *in = static_cast<struct curlFile *>(cfile);
    
    if(in->stream == nullptr) {
        in->stream = fopen(in->filename.c_str(), "in");
        if(in->stream == nullptr)
            return -1;
    }
    
    return fread(buffer, size, nmemb, in->stream);
}

Socket::Socket() noexcept : target({}), curl(nullptr) {
    curl_global_init(CURL_GLOBAL_SSL);
}

Socket::~Socket() noexcept {
    close();
    curl_global_cleanup();
}
        
int Socket::open(const std::string &path) noexcept {
    return open("file", {}, path);
}

int Socket::open(const std::string &protocol,
                 const std::string &server) noexcept {
    return open(protocol, server, {});
}

int Socket::open(const std::string &protocol, const std::string &server,
                 const std::string &path) noexcept {
    target = protocol + "://";
    if (! server.empty())
       target += server;
    if ( (! path.empty()) && (! server.empty()) ) 
        target += '/';
    if (! path.empty())
        target += path;

    close(); 
    curl = curl_easy_init();
    
    if (curl != nullptr) {
        /* Extract the port from the path (if any) */
        const auto pos = target.find_last_of(':');
        if (pos != std::string::npos) {
            const auto port = target.substr(pos+1);
            if (std::all_of(port.begin(), port.end(), ::isdigit)) {
                target = target.substr(0, pos);
                auto p = std::stol(port);
                if (p > 0) {
                    curl_easy_setopt(curl, CURLOPT_PORT, p);
                }
            }
        }

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

        return CURLE_OK;
    } else {
        return CURLE_FAILED_INIT;
    }
}

int Socket::setup(const std::string &username,
                  const std::string &password) noexcept {
    return setup(username, password, -1);
}

int Socket::setup(const std::string &username, const std::string &password,
                  long timeout) noexcept {
    return setup(username, password, timeout, -1);
}
         
int Socket::setup(const std::string &username, const std::string &password,
                  long timeout, long port) noexcept {
    ASSERT((curl != nullptr), 
           "Socket::setup(): Cannot setup an unopened socket!");

    if (!username.empty()) {
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
    }
    if (timeout > 0)
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout); 
    if (port > 0)
        curl_easy_setopt(curl, CURLOPT_PORT, port); 
    curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
 
    return CURLE_OK;
}

int Socket::get(const std::string &file) const noexcept {
    return get(".", file);
}

int Socket::get(const std::string &folder,
                const std::string &file) const noexcept { 
    ASSERT((curl != nullptr),
           "Socket::get(): Cannot read from an unopened socket!");

    std::string     input(target + '/' + file);
    struct curlFile output = { folder + '/' + file, nullptr };    
    CURLcode        error;

    curl_easy_setopt(curl, CURLOPT_URL, input.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFile);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);

    error = curl_easy_perform(curl);

    if (output.stream != nullptr) {
        fclose(output.stream);
    }

    return error;
}

int Socket::get(std::vector<unsigned char> &stream) const noexcept { 
    return get(stream, {});
}

int Socket::get(std::vector<unsigned char> &stream,
                const std::string &file) const noexcept { 
    ASSERT((curl != nullptr),
           "Socket::get(): Cannot read from an unopened socket!");

    std::string input(target);
    if (!file.empty()) {
        //input += '/' + file;
        input = file;
    }

    curl_easy_setopt(curl, CURLOPT_URL, input.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteStream);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);

    return curl_easy_perform(curl);
}

int Socket::put(const std::string &file) const noexcept {
    return put(".", file);
}

int Socket::put(const std::string &folder,
                const std::string &file) const noexcept {
    ASSERT((curl != nullptr),
           "Socket::put(): Cannot write to an unopened socket!");

    struct curlFile input = { folder + '/' + file, nullptr };    
    std::string     output(target + '/' + file);
    CURLcode        error;

    curl_easy_setopt(curl, CURLOPT_URL, output.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, curlReadFile);
    curl_easy_setopt(curl, CURLOPT_READDATA, &input);

    error = curl_easy_perform(curl);

    if (input.stream != nullptr) {
        fclose(input.stream);
    }

    return error;
}

int Socket::close() noexcept {
    if (curl != nullptr) {
        curl_easy_cleanup(curl);
        curl = nullptr;
    }

    return CURLE_OK;
}

#define BUFFSIZE 16384
std::string MD5::compute(const std::string& file) noexcept {
    char buffer[BUFFSIZE];
    unsigned char digest[MD5_DIGEST_LENGTH];
    std::stringstream ss;
    std::string md5string;
    std::ifstream ifs(file, std::ifstream::binary);
    if (!ifs.good()) return {};

    MD5_CTX md5Context;
    MD5_Init(&md5Context);
    
    while (ifs.good()) {
        ifs.read(buffer, BUFFSIZE);
        MD5_Update(&md5Context, buffer, ifs.gcount());
    }
    
    ifs.close();

    int res = MD5_Final(digest, &md5Context);   
    if (res == 0) return {};

    ss << std::hex << std::setfill('0');
    for(unsigned char uc: digest)
        ss << std::setw(2) << static_cast<int>(uc);
    md5string = ss.str();

    return md5string;
}

}  // namespace Customisation
