#pragma once
/// \file
///
/// Angle conversions.
#include <hm3/math/constants.hpp>
#include <hm3/types.hpp>

namespace hm3::math {

/// Degree to radian conversion
constexpr num_t deg_to_rad(num_t angle_deg) noexcept {
  return angle_deg * (2. * pi) / 360.;
}

/// Radian to degree conversion
constexpr num_t rad_to_deg(num_t angle_rad) noexcept {
  return angle_rad * 360. / (2. * pi);
}

}  // namespace hm3::math
