# Copyright Louis Dionne 2015
# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
#
# Setup compiler flags (more can be set on a per-target basis or in
# subdirectories)

# Compilation flags
include(CheckCXXCompilerFlag)
macro(hm3_append_flag testname flag)
    check_cxx_compiler_flag(${flag} ${testname})
    if (${testname})
        add_compile_options(${flag})
    endif()
endmacro()

# All compilation flags
# Language flag: version of the C++ standard to use
hm3_append_flag(HM3_HAS_STDCXX1Z -std=c++1z)

# PITA warning flags:
hm3_append_flag(HM3_HAS_WSHADOW -Wshadow)
hm3_append_flag(HM3_HAS_WUNUSED -Wunused)
hm3_append_flag(HM3_HAS_WUNUSED_FUNCTION -Wunused-function)
hm3_append_flag(HM3_HAS_WUNUSED_LABEL -Wunused-label)
hm3_append_flag(HM3_HAS_WUNUSED_PARAMETER -Wunused-parameter)
hm3_append_flag(HM3_HAS_WUNUSED_VALUE -Wunused-value)
hm3_append_flag(HM3_HAS_WUNUSED_VARIABLE -Wunused-variable)
hm3_append_flag(HM3_HAS_WMICROSOFT -Wmicrosoft)

# Warning flags:
hm3_append_flag(HM3_HAS_WALL -Wall)
hm3_append_flag(HM3_HAS_WEXTRA -Wextra)
hm3_append_flag(HM3_HAS_WDEPRECATED -Wdeprecated)
hm3_append_flag(HM3_HAS_WDOCUMENTATION -Wdocumentation)
hm3_append_flag(HM3_HAS_WCOMMENT -Wcomment)
hm3_append_flag(HM3_HAS_PEDANTIC -Wpedantic)
hm3_append_flag(HM3_HAS_WSTACK_PROTECTOR -Wstack-protector)
hm3_append_flag(HM3_HAS_WSTRICT_ALIASING "-Wstrict-aliasing=2")
hm3_append_flag(HM3_HAS_WSTRICT_OVERFLOW "-Wstrict-overflow=5")
hm3_append_flag(HM3_HAS_WDISABLED_OPTIMIZATION -Wdisabled-optimization)
hm3_append_flag(HM3_HAS_WINLINE -Winline)
hm3_append_flag(HM3_HAS_WRETURN_TYPE -Wreturn-type)
hm3_append_flag(HM3_HAS_WCAST_ALIGN -Wcast-align)
hm3_append_flag(HM3_HAS_WCAST_QUAL -Wcast-qual)
hm3_append_flag(HM3_HAS_WSIGN_COMPARE -Wsign-compare)
hm3_append_flag(HM3_HAS_WSIGN_PROMO -Wsign-promo)
hm3_append_flag(HM3_HAS_WFORMAT "-Wformat=2")
hm3_append_flag(HM3_HAS_WFORMAT_NONLITERAL -Wformat-nonliteral)
hm3_append_flag(HM3_HAS_WFORMAT_SECURITY -Wformat-security)
hm3_append_flag(HM3_HAS_WFORMAT_Y2K -Wformat-y2k)
hm3_append_flag(HM3_HAS_WMISSING_BRACES -Wmissing-braces)
hm3_append_flag(HM3_HAS_WMISSING_FIELD_INITIALIZERS -Wmissing-field-initializers)
#hm3_append_flag(HM3_HAS_WMISSING_INCLUDE_DIRS -Wmissing-include-dirs)
hm3_append_flag(HM3_HAS_WOVERLOADED_VIRTUAL -Woverloaded-virtual)
hm3_append_flag(HM3_HAS_WCHAR_SUBSCRIPTS -Wchar-subscripts)
hm3_append_flag(HM3_HAS_WFLOAT_EQUAL -Wfloat-equal)
hm3_append_flag(HM3_HAS_WPOINTER_ARITH -Wpointer-arith)
hm3_append_flag(HM3_HAS_WINVALID_PCH -Winvalid-pch)
hm3_append_flag(HM3_HAS_WIMPORT -Wimport)
hm3_append_flag(HM3_HAS_WINIT_SELF -Winit-self)
hm3_append_flag(HM3_HAS_WREDUNDANT_DECLS -Wredundant-decls)
hm3_append_flag(HM3_HAS_WPACKED -Wpacked)
hm3_append_flag(HM3_HAS_WPARENTHESES -Wparentheses)
hm3_append_flag(HM3_HAS_WSEQUENCE_POINT -Wsequence-point)
hm3_append_flag(HM3_HAS_WSWITCH -Wswitch)
hm3_append_flag(HM3_HAS_WSWITCH_DEFAULT -Wswitch-default)
hm3_append_flag(HM3_HAS_WTRIGRAPHS -Wtrigraphs)
hm3_append_flag(HM3_HAS_WUNINITIALIZED -Wuninitialized)
hm3_append_flag(HM3_HAS_WUNKNOWN_PRAGMAS -Wunknown-pragmas)
hm3_append_flag(HM3_HAS_WUNREACHABLE_CODE -Wunreachable-code)
hm3_append_flag(HM3_HAS_WVARIADIC_MACROS -Wvariadic-macros)
hm3_append_flag(HM3_HAS_WVOLATILE_REGISTER_VAR -Wvolatile-register-var)
hm3_append_flag(HM3_HAS_WWRITE_STRINGS -Wwrite-strings)
hm3_append_flag(HM3_HAS_WNO_ATTRIBUTES -Wno-attributes)
hm3_append_flag(HM3_HAS_WUNNEEDED_INTERNAL_DECLARATION -Wunneeded-internal-declaration)

# Template diagnostic flags
hm3_append_flag(HM3_HAS_FDIAGNOSTIC_SHOW_TEMPLATE_TREE -fdiagnostics-show-template-tree)
hm3_append_flag(HM3_HAS_FTEMPLATE_BACKTRACE_LIMIT "-ftemplate-backtrace-limit=0")
hm3_append_flag(HM3_HAS___EXTERN_ALWAYS_INLINE -D__extern_always_inline=inline)

# Eigen3 Flags
hm3_append_flag(HM3_HAS_EIGEN_NO_AUTOMATIC_RESIZING -DEIGEN_NO_AUTOMATIC_RESIZING)
hm3_append_flag(HM3_HAS_EIGEN_STACK_ALLOCATION_LIMIT "-DEIGEN_STACK_ALLOCATION_LIMIT=0")

# Compiler flags controlled by CMake options

if (HM3_ENABLE_WERROR)
  hm3_append_flag(HM3_HAS_WERROR -Werror)
  hm3_append_flag(HM3_HAS_WX -WX)
endif()

if (HM3_ENABLE_ASAN)
  set(HM3_ASAN_FLAGS "-fsanitize=address,integer,undefined,leak")
  #set(HM3_ASAN_FLAGS "-fsanitize=address,integer,undefined -fno-sanitize-recover=address,integer,undefined -fsanitize-blacklist=${PROJECT_SOURCE_DIR}/sanitize.blacklist")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HM3_ASAN_FLAGS}")
  hm3_append_flag(HM3_HAS_SANITIZE_FLAGS "-fsanitize=address,integer,undefined")
  hm3_append_flag(HM3_HAS_SANITIZE_NO_RECOVER "-fno-sanitize-recover=address,integer,undefined")
  hm3_append_flag(HM3_HAS_SANITIZE_BLACKLIST "-fsanitize-blacklist=${PROJECT_SOURCE_DIR}/sanitize.blacklist")
  hm3_append_flag(HM3_HAS_NO_OMIT_FRAME_POINTER -fno-omit-frame-pointer)
else()
  hm3_append_flag(HM3_HAS_OMIT_FRAME_POINTER -fomit-frame-pointer)
endif()

if (HM3_ENABLE_MSAN)
  set(HM3_MSAN_FLAGS "-fsanitize=memory -fsanitize-recover=all")
  hm3_append_flag(HM3_HAS_MSAN "${HM3_MSAN_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HM3_MSAN_FLAGS}")
  hm3_append_flag(HM3_HAS_NO_OMIT_FRAME_POINTER -fno-omit-frame-pointer)
endif()

if (HM3_ENABLE_DEBUG_INFORMATION)
  hm3_append_flag(HM3_HAS_G3 -g3)
  # Optimize debug info for gdb:
  # TODO: control this with flags: HM3_ENABLE_GDB_DEBUG_INFORMATION/ _LLDB_...
  hm3_append_flag(HM3_HAS_GGDB -ggdb)
else()
  hm3_append_flag(HM3_HAS_G0 -g0)
endif()

if (NOT HM3_ENABLE_ASSERTIONS)
  hm3_append_flag(HM3_HAS_DHM3_DISABLE_ASSERTIONS -DHM3_DISABLE_ASSERTIONS)
endif()

if (HM3_ENABLE_COVERAGE)
  if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(FATAL_ERROR "code coverage instrumentation requires CMAKE_BUILD_TYPE=Debug")
  endif()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()

# Optimization flags: debug
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  hm3_append_flag(HM3_HAS_O0 -O0)
  hm3_append_flag(HM3_HAS_NO_INLINE -fno-inline)
  hm3_append_flag(HM3_HAS_STACK_PROTECTOR_ALL -fstack-protector-all)
  hm3_append_flag(HM3_HAS_EIGEN_FAST_MATH0 "-DEIGEN_FAST_MATH=0")
  hm3_append_flag(HM3_HAS_EIGEN_DONT_VECTORIZE -DEIGEN_DONT_VECTORIZE)
  hm3_append_flag(HM3_HAS_EIGEN_DONT_PARALLELIZE -DEIGEN_DONT_PARALLELIZE)
  hm3_append_flag(HM3_HAS_NO_STRICT_ALIASING -fno-strict-aliasing)
endif()

# Optimization flags: release
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  hm3_append_flag(HM3_HAS_OFAST -Ofast)
  hm3_append_flag(HM3_HAS_OFAST -fstrict-vtable-pointers)
  hm3_append_flag(HM3_HAS_EIGEN_FAST_MATH "-DEIGEN_FAST_MATH=1")
  hm3_append_flag(HM3_HAS_DEFINE_FAST_MATH "-D__FAST_MATH__")
  #hm3_append_flag(HM3_HAS_SIZED_DEALLOCATION -fsized-deallocation)
  set(HM3_POLLY_FLAGS "-mllvm -polly -mllvm -polly-vectorizer=stripmine")
  check_cxx_compiler_flag($HM3_POLLY_FLAGS HM3_HAS_POLLY)
  if(HM3_HAS_POLLY)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HM3_POLLY_FLAGS}")
  endif()
  hm3_append_flag(HM3_HAS_MARCH_NATIVE "-march=native")
  hm3_append_flag(HM3_HAS_MTUNE_NATIVE "-mtune=native")
  hm3_append_flag(HM3_HAS_STRICT_ALIASING -fstrict-aliasing)
  hm3_append_flag(HM3_HAS_VECTORIZE -fvectorize)
  # If assertions disabled, disable assertions in std lib and eigen
  if (NOT HM3_ENABLE_ASSERTIONS)
    hm3_append_flag(HM3_HAS_DNDEBUG -DNDEBUG)
    hm3_append_flag(HM3_HAS_EIGEN_NO_DEBUG -DEIGEN_NO_DEBUG)
  endif()
  # If ASan and profile not enabled: omit frame pointer
  if (NOT HM3_ENABLE_ASAN AND NOT HM3_ENABLE_PROFILE)
    hm3_append_flag(HM3_HAS_OMIT_FRAME_POINTER -fomit-frame-pointer)
  endif()
  # Others could be: -pipe pfdata-sections -ffunction-sections
  # for clang: -mllvm -inline-threshold=X (X=1000, 10000, ...)
endif()

if (NOT HM3_ENABLE_LIKELY)
  hm3_append_flag(HM3_HAS_DISABLE_LIKELY -DHM3_DISABLE_LIKELY_MACROS)
endif()

if (NOT HM3_ENABLE_RESTRICT)
  hm3_append_flag(HM3_HAS_DISABLE_RESTRICT -DHM3_DISABLE_RESTRICT_MACRO)
endif()
