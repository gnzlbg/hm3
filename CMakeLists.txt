cmake_minimum_required (VERSION 3.0)
project(H3 CXX)

# Directories:
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/build_system)
set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)

# Options:
include(CMakeDependentOption)
option(H3_WITH_ASAN   "Build with Address Sanitizer." ON)
option(H3_ENABLE_WERROR   "Warnings as errors." OFF)
option(H3_ENABLE_ASSERTIONS "Warnings as errors." ON)
option(H3_ENABLE_DEBUG_INFO "Enables debug information." ON)
option(H3_VERBOSE_CONFIGURE "Enables verbose configure output." ON)
option(H3_COLLECT_COVERAGE "Builds with coverage instrumentation." OFF)
option(H3_WITH_DOCS "Use Doxygen to create the HTML documentation." ON)

# Compiler flags:
set(H3_CXX_FL "-std=c++1z" "-Wall" "-Wextra" "-Wdocumentation" "-pedantic"
  "-Woverloaded-virtual" "-Wcast-align" "-Wcomment" "-Wcast-qual" "-Wshadow"
  "-Wchar-subscripts" "-Wdisabled-optimization" "-Wfloat-equal" "-Wformat=2"
  "-Winvalid-pch" "-Wformat-nonliteral" "-Wformat-security" "-Wformat-y2k"
  "-Wimport" "-Winit-self" "-Winline" "-Wreturn-type" "-Wmissing-braces"
  "-Wmissing-field-initializers" "-Wmissing-include-dirs" "-Wredundant-decls"
  "-Wpacked" "-Wparentheses" "-Wpointer-arith" "-Wsequence-point"
  "-Wsign-compare" "-Wstack-protector" "-Wstrict-aliasing=2" "-Wswitch"
  "-Wswitch-default" "-Wtrigraphs" "-Wuninitialized" "-Wunknown-pragmas"
  "-Wunreachable-code" "-Wunused" "-Wunused-function" "-Wunused-label"
  "-Wunused-parameter" "-Wunused-value" "-Wunused-variable"
  "-Wvariadic-macros" "-Wvolatile-register-var" "-Wwrite-strings"
  "-Woverloaded-virtual" "-Wsign-promo" "-Wstrict-overflow=5"
  "-fdiagnostics-show-template-tree" "-ftemplate-backtrace-limit=0"
  "-Wno-attributes" "-Wdeprecated" "-Wunneeded-internal-declaration")

if (H3_ENABLE_WERROR)
  message("[O] Warnings will be treated as errors.")
  list(APPEND H3_CXX_FL "-Werror" "-pedantic-errors")
endif()

if (H3_ENABLE_ASSERTIONS)
  message("[O] Assertions enabled.")
else()
  message("[O] Assertions disabled.")
  list(APPEND H3_CXX_FL "-DNDEBUG -DHOM3_DISABLE_ASSERTIONS -DEIGEN_NO_DEBUG")
endif()

if (H3_WITH_ASAN)
  message("[O] Address Sanitizer enabled.")
  list(APPEND H3_CXX_FL "-fsanitize=address,integer,undefined" 
    "-fno-omit-frame-pointer" "-fno-sanitize=unsigned-integer-overflow")
endif()

if (H3_COLLECT_COVERAGE)
  if(NOT H3_ENABLE_DEBUG_INFO)
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
      message("[Error] Coverage analysis requires debug information.")
      exit()
    endif()
  endif()
  message("[O] Collecting coverage information.")
  list(APPEND H3_CXX_FL "--coverage" "-Xclang -coverage-cfg-checksum"
    "-Xclang -coverage-no-function-names-in-data"
    "-Xclang -coverage-version='407*'")
endif()

#  Debug optimization flags:
set(H3_DBG_FL "-O0" "-fno-inline" "-fstack-protector-all")
set(H3_REL_FL "-Ofast" "-march=native" "-mtune=native"
  "-fstrict-aliasing" "-pipe" "-fdata-sections" "-ffunction-sections"
  "-fvectorize" "-fslp-vectorize-aggressive")
if(H3_ENABLE_DEBUG_INFO)
  message("[O] Debug information enabled.")
  list(APPEND H3_CXX_FL "-g3")
else()
  message("[O] Debug information disabled.")
  list(APPEND H3_CXX_FL "-g0")
endif()
if (NOT H3_WITH_ASAN)
  list(APPEND H3_CXX_FL "-fomit-frame-pointer")
endif()

#  Specify dependencies
macro(h3_dep name cflags lflags)
  find_package(${name} REQUIRED)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${name}_COMPILE_FLAGS} ${cflags}")
  set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} ${${name}_LINK_FLAGS} ${lflags}")
  include_directories(SYSTEM ${${name}_INCLUDE_DIR})
endmacro()

# Dependencies
h3_dep(MPI "" "")
h3_dep(EIGEN3 "-DEIGEN_FAST_MATH -DEIGEN_NO_AUTOMATIC_RESIZING" "")
h3_dep(HDF5 "" "")
h3_dep(HDF5LITE "" "")
h3_dep(TBB "" "")
h3_dep(THRUST "-DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_TBB" "")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.53 COMPONENTS mpi serialization filesystem system log regex
                                   date_time thread chrono REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

set(H3_LIBS ${Boost_LIBRARIES} ${MPI_LIBRARIES} ${HDF5_LIBRARY}
    ${HDF5LITE_LIBRARY} ${TBB_LIBRARIES})

# Set compiler flags:
include(join)
join(H3_CXX_FL " " ${H3_CXX_FL})
join(H3_DBG_FL " " ${H3_DBG_FL})
join(H3_REL_FL " " ${H3_REL_FL})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${H3_CXX_FL}")
set(CMAKE_CXX_FLAGS_DEBUG "${H3_DBG_FL}")
set(CMAKE_CXX_FLAGS_RELEASE "${H3_REL_FL}")

# Create compilation data-base:
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE include_files "include/*.hpp")
file(GLOB_RECURSE test_header_files "test/*.hpp")
file(GLOB_RECURSE test_src_files "test/*.cpp")
set(include_file_list ${include_files})
set(fmt_file_list ${include_files})
list(APPEND fmt_file_list ${test_header_files} ${test_src_files})

string(REPLACE ";" " " include_file_list "${include_file_list}")
string(REPLACE ";" " " fmt_file_list "${fmt_file_list}")

# Clang-format: make fmt
add_custom_command(OUTPUT formatting-update COMMAND
  ${PROJECT_SOURCE_DIR}/build_system/update_format.sh ${fmt_file_list})
add_custom_target(fmt DEPENDS formatting-update WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# LCov (coverage): make cov
add_custom_command(OUTPUT coverage COMMAND
  ${PROJECT_SOURCE_DIR}/build_system/coverage.sh "${PROJECT_SOURCE_DIR}/include")
add_custom_target(cov DEPENDS coverage)

# Verbose output:
if(H3_VERBOSE_CONFIGURE)
  message("[O] Build type: ${CMAKE_BUILD_TYPE}")
  message("The compiler is: ${CMAKE_CXX_COMPILER}")
  message("CXX:")
  message(" * Flags: ${CMAKE_CXX_FLAGS}")
  message(" * Debug Flags: ${CMAKE_CXX_FLAGS_DEBUG}")
  message(" * Release Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
  message(" * Compile Flags: ${CMAKE_CXX_COMPILE_FLAGS}")
  message("C:")
  message(" * Flags: ${CMAKE_C_FLAGS}")
  message(" * Compile Flags: ${CMAKE_C_COMPILE_FLAGS}")
  message("Linker flags:")
  message(" * exe:${CMAKE_EXE_LINKER_FLAGS}")
  message(" * cxx: ${CMAKE_CXX_LINK_FLAGS}")
  message(" * module: ${CMAKE_MODULE_LINKER_FLAGS}")
  get_directory_property(CMakeCompDirDefs COMPILE_DEFINITIONS)
  message("Definitions: ${CmakeCompDirDefs}")
endif()

# Subdirectories
include_directories(include)
add_subdirectory(doc)
add_subdirectory(test)
