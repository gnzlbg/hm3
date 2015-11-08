#pragma once
/// \file
///
/// Aligned pointer macro: tells the compiler that the memory this pointer
/// points to is aligned to a particular alignment.
///
/// Example:
///
///   // without align:
///   double* ptr = other_ptr;
///
///   // with align and a specific alignment:
///   double* ptr = HM3_ALIGNED_PTR_TO(other_ptr, sizeof(double));
///
///   // with align and aligned to sizeof(remove_ptr_t<decltype(other_ptr)>)
///   double* ptr = HM3_ALIGNED_PTR(other_ptr);
///
#include <hm3/utility/compiler.hpp>

#ifdef HM3_DISABLE_ALIGNED_POINTER_MACRO
// Disable aligned pointer:
#define HM3_ALIGNED_PTR_TO(ptr, alignment)
#define HM3_ALIGNED_PTR(ptr)
#else  // Otherwise enable aligned pointer for each compiler

// For each compiler
#if defined(HM3_COMPILER_GCC) || defined(HM3_COMPILER_CLANG)

#define HM3_ALIGNED_PTR_TO(ptr, alignment) \
  static_cast<decltype(ptr)>(__builtin_assume_aligned((ptr), alignment))

#define HM3_ALIGNED_PTR(ptr)                           \
  static_cast<decltype(ptr)>(__builtin_assume_aligned( \
   (ptr), sizeof(std::remove_pointer_t<decltype(ptr)>)))

#else
#error HM3_ALIGNED_POINTER is not implemented for this compiler.
#endif  // For each compiler

#endif  // HM3_ALIGNED_POINTER_MACRO
