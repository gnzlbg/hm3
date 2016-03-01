#pragma once
/// \file
///
/// Fatal error
///
/// TODO: this should be configurable to e.g. throw instead of terminate.
#include <hm3/utility/config/at.hpp>
#include <hm3/utility/fmt.hpp>

// This requires the gnu/clang zero-variadic-macro-arguments language extension
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

/// Produces a fatal error
#define HM3_FATAL_ERROR_AT(at, message, ...)                                  \
  [[ cold, noinline ]] do {                                                   \
    ::hm3::fmt::print(stderr, "\nFATAL ERROR:\n\n  message: " message "\n\n", \
                      ##__VA_ARGS__);                                         \
    HM3_PRINT_AT((at));                                                       \
    std::terminate();                                                         \
  }                                                                           \
  while (false)

/// Produces a fatal error
#define HM3_FATAL_ERROR(message, ...)                                         \
  [[ cold, noinline ]] do {                                                   \
    ::hm3::fmt::print(stderr, "\nFATAL ERROR:\n\n  message: " message "\n\n", \
                      ##__VA_ARGS__);                                         \
    HM3_PRINT_AT(HM3_AT_);                                                    \
    std::terminate();                                                         \
  }                                                                           \
  while (false)

#pragma clang diagnostic pop
