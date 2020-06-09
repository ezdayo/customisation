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
project(Customisation
        LANGUAGES CXX)

# The project version number.
set(VERSION_MAJOR   1   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR} rev. ${VERSION_PATCH}")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH VERSION_FULL)

# CMake version 3.1+ is required (to enable C++11 options)
# CMake version 3.6+ is required (to enable clang-tidy commands)
# CMake version 3.10+ is required (to enable cppcheck commands)
cmake_minimum_required(VERSION 3.1)

# C++ compiler options for all build configurations 
set (CMAKE_CXX_STANDARD 11)
include(mk/cxx-flag-rules.cmake)
enable_cxx_compiler_flag_if_supported(-Wall)
enable_cxx_compiler_flag_if_supported(-Wextra)
enable_cxx_compiler_flag_if_supported(-pedantic)
enable_cxx_compiler_flag_if_supported(-fvisibility=hidden)
enable_cxx_compiler_flag_if_supported(-fdata-sections)
enable_cxx_compiler_flag_if_supported(-ffunction-sections)
enable_cxx_compiler_flag_if_supported("${EXTRA_CXX_FLAGS}")

if(!ANDROID)
SET(CMAKE_AR  "gcc-ar")
SET(CMAKE_RANLIB  "gcc-ranlib")
endif()

#
# Binary targets
#

# Configure header files
configure_file (${PROJECT_SOURCE_DIR}/tpl/customisation.hpp.in
	        ${PROJECT_SOURCE_DIR}/inc/customisation.hpp @ONLY)

# Customisation static library configuration
include_directories(${PROJECT_SOURCE_DIR}/inc)
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

# Targets definitions
add_library(customisation STATIC ${SRC_FILES})
target_link_libraries(customisation -Wl,--gc-sections)
install(TARGETS customisation 
	ARCHIVE DESTINATION lib)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/inc/
	DESTINATION include
	FILES_MATCHING PATTERN "*.hpp")

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

