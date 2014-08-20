#pragma once
/// \file error.hpp Error-handling utilities
#include <stdexcept>
#include <hm3/types.hpp>

namespace hm3 {

/// Error-handling utilities
namespace error {

/// Throws a runtime error exception with the description given
/// in \p error_string.
inline std::runtime_error exception(string const& error_string) {
  std::runtime_error tmp(error_string);
  return tmp;
}

}  // namespace error
}  // namespace hm3
