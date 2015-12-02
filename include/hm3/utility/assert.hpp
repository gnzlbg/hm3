#pragma once
/// \file
///
/// Assert macro
#include <hm3/utility/assume.hpp>
#include <hm3/utility/at.hpp>
#include <hm3/utility/fmt.hpp>
#include <hm3/utility/likely.hpp>
#include <type_traits>

// This requires the gnu/clang zero-variadic-macro-arguments language extension
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

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
      ::hm3::fmt::print(                                                   \
       stderr,                                                             \
       "\nASSERTION FAILED:\n\n  condition: \"{}\"\n  message: \"" message \
       "\"\n\n",                                                           \
       #condition, ##__VA_ARGS__);                                         \
      HM3_PRINT_AT(AT);                                                    \
      __builtin_debugtrap();                                               \
    }                                                                      \
    HM3_ASSUME((condition));                                               \
  } while (false)
#else  // HM3_DISABLE_ASSERTIONS
#define HM3_ASSERT_AT(condition, message, AT, ...)                         \
  do {                                                                     \
    if (false && HM3_UNLIKELY(!(condition))) {                             \
      ::hm3::fmt::print(                                                   \
       stderr,                                                             \
       "\nASSERTION FAILED:\n\n  condition: \"{}\"\n  message: \"" message \
       "\"\n",                                                             \
       #condition, ##__VA_ARGS__);                                         \
      HM3_PRINT_AT(AT);                                                    \
      __builtin_debugtrap();                                               \
    }                                                                      \
    HM3_ASSUME((condition));                                               \
  } while (false)
#endif  // HM3_DISABLE_ASSERTIONS

#define HM3_ASSERT(condition, message, ...) \
  HM3_ASSERT_AT(condition, message, HM3_AT_, ##__VA_ARGS__)

#pragma clang diagnostic pop

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
