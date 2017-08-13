#pragma once
/// \file
///
/// AT macro used to get a source file location (file, line, function)
#include <hm3/types.hpp>
#include <hm3/config/attributes.hpp>

namespace hm3 {

#ifndef HM3_DISABLE_ASSERTIONS
struct source_location {
  template <typename A, typename B, typename C>
  [[HM3_ALWAYS_INLINE]] constexpr source_location(A&& func, B&& file, C&& line)
   : function_name(static_cast<const char*>(func))
   , file_name(static_cast<const char*>(file))
   , line_number(line) {}

  const char* const function_name;
  const char* const file_name;
  const int line_number;
};

/// Returns a source code locator containing the position where it is
/// defined.
#define HM3_AT_ ::hm3::source_location(__PRETTY_FUNCTION__, __FILE__, __LINE__)

#else  // ifdef HM3_DISABLE_ASSERTIONS

struct source_location {
  const char* const function_name = "";
  const char* const file_name     = "";
  const int line_number           = -1;
};

#define HM3_AT_ ::hm3::source_location()

#endif  // HM3_DISABLE_ASSERTIONS

}  // namespace hm3

/// Formats a source code location
#define HM3_FMT_AT(AT)                                                        \
  ::hm3::ascii_fmt::detail::format(                                           \
   "  at function: {}\n  at file: {}\n  at line: {}\n\n", (AT).function_name, \
   (AT).file_name, (AT).line_number)

/// Formats a source code location
#define HM3_PRINT_AT(AT)                                                       \
  ::hm3::ascii_fmt::err("  at function: {}\n  at file: {}\n  at line: {}\n\n", \
                        (AT).function_name, (AT).file_name, (AT).line_number)
