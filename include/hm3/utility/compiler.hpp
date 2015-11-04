#pragma once
/// \file
///
/// Detects the current compiler

#if defined(__GNUC__) && !defined(__clang__)  // detects GCC
#define HM3_COMPILER_GCC
#elif defined(__clang__)  // detects Clang
#define HM3_COMPILER_CLANG
#else
#error Unknown compiler
#endif
