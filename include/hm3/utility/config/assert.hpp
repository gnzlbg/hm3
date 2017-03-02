#pragma once
/// \file
///
/// Assert macro
#include <hm3/utility/config/assume.hpp>
#include <hm3/utility/config/at.hpp>
#include <hm3/utility/config/compiler.hpp>
#include <hm3/utility/config/debug_trap.hpp>
#include <hm3/utility/config/likely.hpp>
#include <hm3/utility/config/unreachable.hpp>
#include <hm3/utility/fmt.hpp>
#include <type_traits>

// This requires the gnu/clang zero-variadic-macro-arguments language extension
#ifdef HM3_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

#ifdef HM3_COMPILER_GCC
#pragma GCC diagnostic push
// This doesn't work for some reason, and there is no warning flag to disable
// this particular warning in GCC 5.3 =/
// #pragma GCC diagnostic ignored "-Wpedantic"
// This is overkill but works:
#pragma GCC system_header
#endif

/// Asserts a condition in DEBUG mode. Useful for checking pre/postconditions
/// and invariants.
///
/// \warning if HM3_DISABLE_ASSERTIONS is defined the condition in the if clause
/// is always false and HM3_ASSERT compiles to nothing! Still this happens after
/// the compiler has verified the correctness of the HM3_ASSERT code.
#ifndef HM3_DISABLE_ASSERTIONS
#define HM3_ASSERT_AT(condition, message, AT, ...)                         \
  do {                                                                     \
    if (HM3_UNLIKELY(!(condition))) {                                      \
      ::hm3::ascii_fmt::err(                                               \
       "\nASSERTION FAILED:\n\n  condition: \"{}\"\n  message: \"" message \
       "\"\n\n",                                                           \
       #condition, ##__VA_ARGS__);                                         \
      HM3_PRINT_AT(AT);                                                    \
      HM3_DEBUG_TRAP();                                                    \
    }                                                                      \
    HM3_ASSUME((condition));                                               \
  } while (false)
#else  // HM3_DISABLE_ASSERTIONS
#define HM3_ASSERT_AT(condition, message, AT, ...)                         \
  do {                                                                     \
    if (false && HM3_UNLIKELY(!(condition))) {                             \
      HM3_UNREACHABLE();                                                   \
      ::hm3::ascii_fmt::err(                                               \
       "\nASSERTION FAILED:\n\n  condition: \"{}\"\n  message: \"" message \
       "\"\n",                                                             \
       #condition, ##__VA_ARGS__);                                         \
      HM3_PRINT_AT(AT);                                                    \
      HM3_DEBUG_TRAP();                                                    \
    }                                                                      \
    HM3_ASSUME((condition));                                               \
  } while (false)
#endif  // HM3_DISABLE_ASSERTIONS

#define HM3_ASSERT(condition, message, ...) \
  HM3_ASSERT_AT(condition, message, HM3_AT_, ##__VA_ARGS__)

#ifdef HM3_COMPILER_CLANG
#pragma clang diagnostic pop
#endif

#ifdef HM3_COMPILER_GCC
#pragma GCC diagnostic pop
#endif

#define HM3_ASSERT_STANDARD_LAYOUT(T)                              \
  static_assert(std::is_standard_layout<T>{}, "");                 \
  static_assert(std::is_pod<T>{}, "");                             \
  static_assert(std::is_literal_type<T>{}, "");                    \
  static_assert(std::is_trivially_constructible<T>{}, "");         \
  static_assert(std::is_trivially_default_constructible<T>{}, ""); \
  static_assert(std::is_trivially_copy_constructible<T>{}, "");    \
  static_assert(std::is_trivially_move_constructible<T>{}, "");    \
  static_assert(std::is_trivially_assignable<T, T>{}, "");         \
  static_assert(std::is_trivially_copy_assignable<T>{}, "");       \
  static_assert(std::is_trivially_move_assignable<T>{}, "");       \
  static_assert(std::is_trivially_destructible<T>{}, "")
