#pragma once
/// \file
///
/// HM3's primitive types (these are used everywhere except when doing I/O)
#include <string>
#include <cstddef>

namespace hm3 {

/// \name Primitive type aliases
///@{

using int_t   = int_fast64_t;
using uint_t  = uint_fast64_t;
using sint_t  = int_fast32_t;
using suint_t = uint_fast32_t;
using num_t   = double;
using string  = std::string;

/// \name Index types
///@{
using idx_t  = int_t;
using sidx_t = sint_t;
///@}  // Index types

constexpr int_t operator"" _i(unsigned long long int i) {
  return static_cast<int_t>(i);
}

constexpr uint_t operator"" _u(unsigned long long int i) {
  return static_cast<uint_t>(i);
}

constexpr suint_t operator"" _su(unsigned long long int i) {
  return static_cast<suint_t>(i);
}

constexpr std::size_t operator"" _z(unsigned long long int t) { return t; }

///@}  // Primitive type aliases

}  // namespace hm3
