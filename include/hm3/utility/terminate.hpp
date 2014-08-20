#pragma once
/// \file terminate.hpp Program termination routines
#include <hm3/utility/at.hpp>
#include <hm3/utility/fmt.hpp>

// This requires the gnu/clang zero-variadic-macro-arguments language extension
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

/// Terminates the program
#define HM3_TERMINATE_AT(at, message, ...)                                    \
  [[ cold, noinline ]] do {                                                   \
    ::hm3::fmt::print(stderr, "\nFATAL ERROR:\n\n  message: " message "\n\n", \
                      ##__VA_ARGS__);                                         \
    HM3_PRINT_AT((at));                                                       \
    std::terminate();                                                         \
  }                                                                           \
  while (false)

/// Terminates the program
#define HM3_TERMINATE(message, ...)                                           \
  [[ cold, noinline ]] do {                                                   \
    ::hm3::fmt::print(stderr, "\nFATAL ERROR:\n\n  message: " message "\n\n", \
                      ##__VA_ARGS__);                                         \
    HM3_PRINT_AT(HM3_AT_);                                                    \
    std::terminate();                                                         \
  }                                                                           \
  while (false)

#pragma clang diagnostic pop
