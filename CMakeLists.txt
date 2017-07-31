# Copyright Gonzalo Brito Gadeschi 2013-2015
# Copyright Louis Dionne 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

# CMake Version:
cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.7.1)
#cmake_policy(SET CMP0066 NEW)

# Setup project
project(hm3 CXX)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export compilation data-base

# Build system
include(hm3_options)
include(hm3_compiler_flags)
include(hm3_packages)
include(hm3_targets)
include(hm3_format)
include(hm3_tidy)

# Include library
include_directories(${hm3_SOURCE_DIR}/include)
enable_testing()

# Setup subdirectories
add_subdirectory(test)
add_subdirectory(site)
add_subdirectory(vis)

# Setup the `check` target to build, check format, and then run all the tests
# and examples.
add_custom_target(check ALL
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS test.unit check-format test.headers tests tests.mpi
  COMMENT "Build and then run all the tests.")
