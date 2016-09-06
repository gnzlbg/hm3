#pragma once
/// \file
///
/// Workarounds std::math functions that are not constexpr
#include <hm3/math/integer.hpp>
#include <type_traits>

namespace hm3::math {

/// Is a floating point number NaN?
///
/// NaNs never compare equal to themselves
///
/// \note Workaround until std::isnan becomes constexpr
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr bool is_nan(Float x) {
  return __builtin_isnan(x);
}

/// Is a floating point number finite?
///
/// \note Workaround until std::isfinite becomes constexpr
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr bool is_finite(Float x) {
  return __builtin_isfinite(x);
}

/// Is a floating point number infinite?
///
/// \note Workaround until std::isinf becomes constexpr
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr bool is_infinite(Float x) {
  return __builtin_isinf(x);
}

/// Integer floor
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr int64_t ifloor(Float x) {
  HM3_ASSERT(is_finite(x), "floor of not finite number {}", x);
  int64_t i = x;
  return i - (x < i);
}

/// Integer ceil
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr int64_t iceil(Float x) {
  HM3_ASSERT(is_finite(x), "ceil of not finite number {}", x);
  int64_t i = x;
  return i + (x > i);
}

/// Constexpr version of cmath std::floor
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr Float floor(Float x) {
  return ifloor(x);
}

/// Constexpr version of cmath std::ceil
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr Float ceil(Float x) {
  return iceil(x);
}

}  // namespace hm3::math
