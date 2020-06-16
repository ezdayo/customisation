#
# These are the CMakeLists for the Customisation library
#
# This script is a CMake script aimed at generating the build environment for
# the Customisation library
#
# This file is part of the Customisation framework (see link).
#
# Author:   Olivier Stoltz-Douchet <ezdayo@gmail.com>
#
# Copyright (c) 2019-2020 Olivier Stoltz-Douchet
# License:  http://opensource.org/licenses/MIT MIT
# Link:     https://github.com/ezdayo/customisation
#

# Customisation is a C++ project
project(Customisation LANGUAGES CXX)

# The project version number.
set(VERSION_MAJOR   1   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR} rev. ${VERSION_PATCH}")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH VERSION_FULL)

# PKGCONFIG installation directory
set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" 
    CACHE PATH "Installation directory for pkgconfig (.pc) files")

# CMake version 3.1+ is required (to enable C++11 options)
# CMake version 3.6+ is required (to enable clang-tidy commands)
# CMake version 3.10+ is required (to enable cppcheck commands)
cmake_minimum_required(VERSION 3.1)

# Runtime library dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/mk)
find_package(PkgConfig)

# The Curl package is mandatory for the configuration IO implementation
pkg_check_modules(Curl libcurl)
pkg_check_modules(OpenSSL libcrypto libssl)

# The readline package is mandatory for the CLI interface implementation
find_package(Readline)

# C++ compiler options for all build configurations 
set (CMAKE_CXX_STANDARD 11)
include(mk/cxx-flag-rules.cmake)
enable_cxx_compiler_flag_if_supported(-Wall)
enable_cxx_compiler_flag_if_supported(-Wextra)
enable_cxx_compiler_flag_if_supported(-pedantic)
enable_cxx_compiler_flag_if_supported(-fvisibility=hidden)
enable_cxx_compiler_flag_if_supported(-fdata-sections)
enable_cxx_compiler_flag_if_supported(-ffunction-sections)
enable_cxx_compiler_flag_if_supported(-fPIC)
enable_cxx_compiler_flag_if_supported("${EXTRA_CXX_FLAGS}")

#
# Necessary for link-time optimisation LTO
#
if(NOT ANDROID)
set(CMAKE_AR  "gcc-ar")
set(CMAKE_RANLIB  "gcc-ranlib")
endif()

#
# Binary targets
#

# Customisation static library configuration
include_directories(${PROJECT_SOURCE_DIR}/inc ${Readline_INCLUDE_DIRS})
add_definitions(-DLOGTAG="Customisation")
set(SRC_FILES ${PROJECT_SOURCE_DIR}/inc/customisation.hpp
	      ${PROJECT_SOURCE_DIR}/src/checker.cpp
	      ${PROJECT_SOURCE_DIR}/src/component.cpp
	      ${PROJECT_SOURCE_DIR}/src/entity.cpp
	      ${PROJECT_SOURCE_DIR}/src/io.cpp
	      ${PROJECT_SOURCE_DIR}/src/parameter.cpp
	      ${PROJECT_SOURCE_DIR}/src/storage.cpp
	      ${PROJECT_SOURCE_DIR}/src/translator.cpp
	      ${PROJECT_SOURCE_DIR}/src/types.cpp)

# Configuration and socket are only available when Curl is installed      
if(Curl_FOUND)
	set(SRC_FILES ${SRC_FILES}
	      	      ${PROJECT_SOURCE_DIR}/src/socket.cpp)
	set(CUSTOMISATION_HAS_SOCKET TRUE)
    	set(CUSTOMISATION_SOCKET_INCLUDE
	    "#include \"customisation/socket.hpp\"")
	set(CUSTOMISATION_LIB_PRIVATE
		"${CUSTOMISATION_LIB_PRIVATE} ${Curl_LDFLAGS}")
if(OpenSSL_FOUND)
	set(SRC_FILES ${SRC_FILES}
            ${PROJECT_SOURCE_DIR}/src/configuration.cpp)
	set(CUSTOMISATION_HAS_CONFIGURATION TRUE)
	set(CUSTOMISATION_CONFIGURATION_INCLUDE
	    "#include \"customisation/configuration.hpp\"")
	set(CUSTOMISATION_LIB_PRIVATE
	    "${CUSTOMISATION_LIB_PRIVATE} ${OpenSSL_LDFLAGS}")
else()
	set(CUSTOMISATION_CONFIGURATION_INCLUDE
	    "/* Rebuild Customisation with OpenSSL to enable Configuration */")
endif()
else()
    	set(CUSTOMISATION_SOCKET_INCLUDE
	    "/* Rebuild Customisation with Curl to enable Socket */")
endif()

# CLI interface is only available when the readline library is available
if(Readline_FOUND)
	set(SRC_FILES ${SRC_FILES}
	      	      ${PROJECT_SOURCE_DIR}/src/cli.cpp)
	set(CUSTOMISATION_HAS_CLI TRUE)
	set(CUSTOMISATION_CLI_INCLUDE "#include \"customisation/cli.hpp\"")
	get_filename_component(Readline_LIB_PATH ${Readline_LIBRARY} DIRECTORY)
	set(CUSTOMISATION_LIB_PRIVATE
	    "${CUSTOMISATION_LIB_PRIVATE} -L${Readline_LIB_PATH} -lreadline")
else()
	set(CUSTOMISATION_CLI_INCLUDE
	    "/* Rebuild Customisation with Readline to enable CLI */")
endif()

# Configure header files
configure_file (${PROJECT_SOURCE_DIR}/tpl/customisation.hpp.in
	        ${PROJECT_SOURCE_DIR}/inc/customisation.hpp @ONLY)

# Configure pkg-config files
configure_file (${PROJECT_SOURCE_DIR}/tpl/customisation.pc.in
	        ${PROJECT_BINARY_DIR}/customisation.pc @ONLY)

# Targets definitions
add_library(customisation STATIC ${SRC_FILES})
target_link_libraries(customisation 
		      ${CURL_LDFLAGS} ${Readline_LIBRARY} -Wl,--gc-sections)
install(TARGETS customisation 
	ARCHIVE DESTINATION lib)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/inc/
	DESTINATION include
	FILES_MATCHING PATTERN "*.hpp"
	PATTERN "*/cli.hpp" EXCLUDE
	PATTERN "*/configuration.hpp" EXCLUDE
	PATTERN "*/socket.hpp" EXCLUDE)

if(Curl_FOUND)
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/inc/
		DESTINATION include
		FILES_MATCHING PATTERN "*/configuration.hpp"
		PATTERN "*/socket.hpp")
endif()

if(Readline_FOUND)
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/inc/
		DESTINATION include
		FILES_MATCHING PATTERN "*/cli.hpp")
endif()

install(FILES ${PROJECT_BINARY_DIR}/customisation.pc 
	DESTINATION "${INSTALL_PKGCONFIG_DIR}")

# Unit tests
add_executable(test-translator EXCLUDE_FROM_ALL
	       ${PROJECT_SOURCE_DIR}/tst/translator.cpp)
target_link_libraries(test-translator customisation -Wl,--gc-sections,-s)

add_executable(test-storage EXCLUDE_FROM_ALL
	       ${PROJECT_SOURCE_DIR}/tst/storage.cpp)
target_link_libraries(test-storage customisation -Wl,--gc-sections,-s)

add_executable(test-checker EXCLUDE_FROM_ALL
	       ${PROJECT_SOURCE_DIR}/tst/checker.cpp)
target_link_libraries(test-checker customisation -Wl,--gc-sections,-s)

add_executable(test-entity EXCLUDE_FROM_ALL
	       ${PROJECT_SOURCE_DIR}/tst/entity.cpp)
target_link_libraries(test-entity customisation -Wl,--gc-sections,-s)

