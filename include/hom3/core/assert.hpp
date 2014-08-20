#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Assert macro
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include <cassert>
#include <iostream>
#include <type_traits>
#include <hom3/core/at.hpp>
#include <hom3/core/likely.hpp>
////////////////////////////////////////////////////////////////////////////////

/// Asserts a condition in DEBUG mode. Useful for checking
/// pre/postconditions and invariants.
///
/// \warning if HOM3_DISABLE_ASSERTIONS is defined the condition in the if
/// clause is always false and ASSERT compiles to nothing! Still this happens
/// after the compiler has verified the correctness of the ASSERT code.
#ifndef HOM3_DISABLE_ASSERTIONS
#define ASSERT_AT(condition, message, AT)                                \
  do {                                                                   \
    if (HOM3_UNLIKELY(!(condition))) {                                   \
      std::cerr << "Assertion `" #condition "` failed in " << AT << ": " \
                << message << std::endl;                                 \
      __builtin_debugtrap();                                             \
    }                                                                    \
  } while (false)
#else
#define ASSERT_AT(condition, message, AT)                                \
  do {                                                                   \
    if (false && (condition)) {                                          \
      std::cerr << "Assertion `" #condition "` failed in " << AT << ": " \
                << message << std::endl;                                 \
      __builtin_debugtrap();                                             \
    }                                                                    \
  } while (false)
#endif

#define ASSERT(condition, message) ASSERT_AT(condition, message, AT_)

namespace hom3 {

/// Asserts that 2 types \p T and \p U are equal
template <class T, class U> inline void assert_equal() noexcept {
  static_assert(std::is_same<T, U>::value, "types are not equal!");
}

}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
