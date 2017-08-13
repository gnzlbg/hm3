#pragma once
/// \file
///
/// Fatal error
///
/// TODO: this should be configurable to e.g. throw instead of terminate, so
/// that the errors do not bring external drivers down (e.g. like ParaView
/// plugins).
#include <hm3/utility/at.hpp>
#include <hm3/utility/cold_do.hpp>
#include <hm3/utility/fmt.hpp>

// This requires the gnu/clang zero-variadic-macro-arguments language extension
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

namespace hm3::detail {}  // namespace hm3::detail

// Note: cold_do prevents the error handling code from getting inlined into the
// parent context, but also prevents the compiler from "seeing" the call to
// `std::terminate` (hence the unreachable hint).

/// Produces a fatal error
#define HM3_FATAL_ERROR_AT(at, message, ...)                              \
  cold_do([&] {                                                           \
    ::hm3::ascii_fmt::err("\nFATAL ERROR:\n\n  message: " message "\n\n", \
                          ##__VA_ARGS__);                                 \
    HM3_PRINT_AT((at));                                                   \
    std::terminate();                                                     \
  });                                                                     \
  HM3_UNREACHABLE()

/// Produces a fatal error
#define HM3_FATAL_ERROR(message, ...)                                     \
  cold_do([&] {                                                           \
    ::hm3::ascii_fmt::err("\nFATAL ERROR:\n\n  message: " message "\n\n", \
                          ##__VA_ARGS__);                                 \
    HM3_PRINT_AT(HM3_AT_);                                                \
    std::terminate();                                                     \
  });                                                                     \
  HM3_UNREACHABLE()

#pragma clang diagnostic pop
