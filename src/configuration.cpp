/**
 *
 * @file      Configuration/configuration.cpp
 *
 * @brief     This is the implementation of Customisation configuration class
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

#include <iostream>
#include <fstream>
#include <iomanip>
#include <openssl/md5.h>
#include <sstream>

#include "log.hpp"
#include "customisation/configuration.hpp"
#include "customisation/socket.hpp"

namespace Customisation {

/* Default external function for user and password request. It simply clears
 * any provided string */
static void none(std::string &s) {
    s.clear();
}

Configuration::Configuration() noexcept
    : Customisation::Entity("Configuration"), protocol("file"), server(""), 
      path(".config"), folder("."), timeout(60), port(-1), 
      user(none), password(none) {
        
    protocol.denominate("protocol")
            .describe("The protocol to access the configuration files")
            .characterise(Customisation::Trait::CONFIGURABLE);
    protocol.allow({ "file", "ftp", "ftps", "http", "https", "scp", "sftp", 
                     "tftp" });
    expose(protocol);

    server.denominate("server")
          .describe("The server URL to access the configuration files")
          .characterise(Customisation::Trait::CONFIGURABLE);
    expose(server);

    path.denominate("path")
        .describe("The full path on the server to access the master "
                  "configuration file")
        .characterise(Customisation::Trait::CONFIGURABLE);
    expose(path);
    
    folder.denominate("folder")
          .describe("The folder where to store the local copies of all "
                    "configuration files. No copy happens when the local "
                    "folder is the same as the input fully qualified path")
          .characterise(Customisation::Trait::CONFIGURABLE);
    expose(folder);

    timeout.denominate("timeout")
           .describe("The server access connection timeout in seconds (-1 for "
                     "infinite timeout)")
           .characterise(Customisation::Trait::CONFIGURABLE);
    timeout.range(-1, 600);
    expose(timeout);
    
    port.denominate("port")
        .describe("The server access port number (-1 for using the default "
                  "protocol port number)")
        .characterise(Customisation::Trait::CONFIGURABLE);
    port.allow({ -1, 20, 21, 22, 23, 69, 80, 443, 981, 989, 990, 8008, 8080 });
    expose(port);
}

Customisation::Error Configuration::setup() noexcept {
    std::string master_name, master_copy, src_folder, dst_folder;

    auto const slash = static_cast<std::string>(path).find_last_of('/');
    master_name = static_cast<std::string>(path).substr(slash+1);
    
    if (master_name.empty()) {
        LOGE("The path shall not be a folder!");
        return Customisation::Error::INVALID_VALUE;
    }
    
    if ( (master_name == ".") || (master_name == "..") ) {
        LOGE("%s[%s]::setup(): Invalid '%s' master configuration name "
             "provided!",
             value_to_string().c_str(), name().c_str(),
             master_name.c_str());
        return Customisation::Error::INVALID_VALUE;
    }
    
    master_copy = master_name + ".old";

    if (slash == std::string::npos) {
        src_folder = "./";
    } else {
        src_folder = static_cast<std::string>(path).substr(0, slash);
    }

    dst_folder = static_cast<std::string>(folder);
    if (dst_folder.empty()) {
        LOGE("%s[%s]::setup(): The setup folder shall not be empty!",
             value_to_string().c_str(), name().c_str());
        return Customisation::Error::INVALID_VALUE;
    }
    if (dst_folder.back() != '/') {
        dst_folder.push_back('/');
    }

    /* The easy bit : check if the source and destination folders are the same
     * so that no copy is necessary and we are done! */
    if ( (static_cast<std::string>(protocol) == "file") && 
         (static_cast<std::string>(server).empty()) && 
         (dst_folder == src_folder) ) {
        return Customisation::Error::NONE;
    }

    Customisation::Socket socket;

    auto error = socket.open(protocol, server, src_folder);
    if (error) {
        LOGE("%s[%s]::setup(): Unable to open IO socket. Error %d",
             value_to_string().c_str(), name().c_str(), error);
        return Customisation::Error::INVALID_VALUE;
    }

    std::string uname, pwd;
    user(uname);
    password(pwd);
    error = socket.setup(uname, pwd, timeout, port);
    if (error) {
        LOGE("%s[%s]::setup(): Unable to setup IO socket. Error %d",
             value_to_string().c_str(), name().c_str(), error);
        return Customisation::Error::INVALID_VALUE;
    }

    std::ifstream src(dst_folder + master_name, std::ios::binary);
    bool has_master = src.good();
    if (has_master) {
        /* Copy the master file not be capable of reverting if required */
        std::ofstream dst(dst_folder + master_copy, std::ios::binary);
        dst << src.rdbuf();
        dst.close();
    }
    src.close();

    error = socket.get(folder, master_name);
    if (error) {
        if (!has_master) {
            LOGE("%s[%s]::setup(): Unable to fetch master configuration "
                 "file '%s'.",
                 value_to_string().c_str(), name().c_str(),
                 master_name.c_str());
            return Customisation::Error::NOT_EXISTING;
        }

        LOGW("%s[%s]::setup(): Unable to fetch master configuration file '%s', "
             "using the previous one.",
             value_to_string().c_str(), name().c_str(),
             master_name.c_str());
        src.open(dst_folder + master_copy, std::ios::binary);
        std::ofstream dst(dst_folder + master_name, std::ios::binary);
        dst << src.rdbuf();
        dst.close();
        src.close();
    }

    std::string md5, file;
    std::ifstream all_cfg(dst_folder + master_name);

    while (all_cfg >> md5 >> file)
    {
        if (MD5::compute(dst_folder + file) == md5)
            continue;
        LOGI("%s[%s]::setup(): Storing '%s' configration file locally", 
             value_to_string().c_str(), name().c_str(),
             file.c_str());
        socket.get(dst_folder, file);
        if (MD5::compute(dst_folder + file) != md5)
        {
            LOGE("%s[%s]::setup(): OOOOPS! Wrong MD5 check for '%s'!",
                 value_to_string().c_str(), name().c_str(),
                 file.c_str());
            return Customisation::Error::INVALID_VALUE;
        }
    }

    return Customisation::Error::NONE;
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
