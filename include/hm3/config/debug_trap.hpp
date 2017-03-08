#pragma once
/// \file
///
/// Debug trap macro
#include <hm3/config/compiler.hpp>

#if defined(HM3_COMPILER_CLANG)
#define HM3_DEBUG_TRAP __builtin_debugtrap
#elif defined(HM3_COMPILER_GCC)
#define HM3_DEBUG_TRAP __builtin_trap
#else
#error HM3_DEBUG_TRAP is not defined for this compiler
#endif
