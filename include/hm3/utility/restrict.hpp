#pragma once
/// \file
///
/// Restrict macro: tells the compiler that the memory this pointer points to
/// will only be accessed through this pointer in the whole program, i.e., there
/// won't be any other pointers/references aliasing it anywhere in the program.
///
/// Example:
///
///     // without restrict:
///     double* ptr = ...;
///
///     // with restrict:
///     double* HM3_RESTRICT ptr = ...;
///
/// The syntax is: <type HM3_RESTRICT variable name ...>
///
#include <hm3/utility/compiler.hpp>

#ifdef HM3_DISABLE_RESTRICT_MACRO
#define HM3_RESTRICT  // Disables restrict
#else  // Otherwise, enable restrict (depends on compiler):

// For each compiler
#if defined(HM3_COMPILER_GCC) || defined(HM3_COMPILER_CLANG)
#define HM3_RESTRICT __restrict
#else
#error HM3_RESTRICT is not implemented for this compiler.
#endif  // For each compiler

#endif  // HM3_DISABLE_RESTRICT_MACRO
