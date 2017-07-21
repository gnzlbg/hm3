#pragma once
/// \file
///
/// Default tolerances
#include <hm3/math/constants.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

// Relative tolerance
#ifndef HM3_GEOMETRY_DISTANCE_RELATIVE_TOLERANCE
#define HM3_GEOMETRY_DISTANCE_RELATIVE_TOLERANCE 1e-15
#endif

// Absolute tolerance
#ifndef HM3_GEOMETRY_DISTANCE_ABSOLUTE_TOLERANCE
#define HM3_GEOMETRY_DISTANCE_ABSOLUTE_TOLERANCE 1e-13
#endif

namespace hm3::geometry {

// TODO: make constexpr
inline num_t next_num(num_t value, uint_t n = 2) noexcept {
  if (n == 0) { return value; }
  while (n > 0) {
    value = std::nextafter(value, math::highest<num_t>);
    --n;
  }
  return value;
}

// TODO: make constexpr
inline num_t prev_num(num_t value, uint_t n = 2) noexcept {
  if (n == 0) { return value; }
  while (n > 0) {
    value = std::nextafter(value, math::lowest<num_t>);
    --n;
  }
  return value;
}

namespace tolerance_detail {

struct tolerance_fn {
  /// Default relative tolerance of distance computations in geometric queries.
  ///
  /// \note Can be globally controlled with the macro
  /// `HM3_GEOMETRY_DISTANCE_RELATIVE_TOLERANCE`.
  static constexpr num_t relative() noexcept {
    return HM3_GEOMETRY_DISTANCE_RELATIVE_TOLERANCE;
  }

  /// Default absolute tolerance of distance computations in geometric queries.
  ///
  /// \note Can be globally controlled with the macro
  /// `HM3_GEOMETRY_DISTANCE_ABSOLUTE_TOLERANCE`.
  static constexpr num_t absolute() noexcept {
    return HM3_GEOMETRY_DISTANCE_ABSOLUTE_TOLERANCE;
  }
};

}  // namespace tolerance_detail

namespace {
constexpr auto const& default_tolerance
 = static_const<tolerance_detail::tolerance_fn>::value;
}

template <typename T>
struct with_default_tolerance : T {
  using T::operator();

  template <typename... Us, typename O = T>
  constexpr auto operator()(Us&&... us) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(O{}(std::forward<Us>(us)...,
                                            default_tolerance.absolute(),
                                            default_tolerance.relative()));
};

}  // namespace hm3::geometry
