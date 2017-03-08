# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
#
# CMake options

include(CMakeDependentOption)

option(HM3_ENABLE_ASAN "Run the tests using AddressSanitizer." OFF)
option(HM3_ENABLE_MSAN "Run the tests using MemorySanitizer." OFF)
option(HM3_ENABLE_ASSERTIONS "Enable assertions." ON)
option(HM3_ENABLE_DEBUG_INFO "Include debug information in the binaries." ON)
option(HM3_ENABLE_COVERAGE "Run the tests with code coverage instrumentation." OFF)
option(HM3_ENABLE_LIKELY "Enables branch-prediction hints (HM3_LIKELY/HM3_UNLIKELY macros)." ON)
option(HM3_ENABLE_PROFILE "Enables compiler flags that help profilers." OFF)
option(HM3_ENABLE_RESTRICT "Enables restrict hint (HM3_RESTRICT macro)." ON)
option(HM3_ENABLE_WERROR "Fail and stop if a warning is triggered." OFF)
option(HM3_ENABLE_PARAVIEW_PLUGINS "Builds ParaView plugins." ON)
option(HM3_ENABLE_VTK "Builds with VTK libraries." OFF)
option(HM3_VERBOSE_CONFIGURE "Prints helpful debug information about CMake scripts." OFF)
option(HM3_ENABLE_DEBUG_INFO_GDB "Include GDB debug information in the binaries." OFF)
option(HM3_ENABLE_DEBUG_INFO_LLDB "Include LLDB debug information in the binaries." OFF)
option(HM3_ENABLE_MODULES "Enables the Modules TS." OFF)

# Enable verbose configure when passing -Wdev to CMake
if (DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS AND
    NOT CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
  set(HM3_VERBOSE_CONFIGURE ON)
endif()
