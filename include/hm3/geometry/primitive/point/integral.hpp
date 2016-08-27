#pragma once
/// \file
///
/// Point integral.
#include <hm3/geometry/algorithm/integral.hpp>
#include <hm3/geometry/primitive/point/point.hpp>

namespace hm3::geometry::point_primitive {

/// Integral over the surface of a point.
///
/// For nd == 1, points are surfaces, and have area == 1.
constexpr auto integral(point<1> const&, surface_integral<point<1>>) noexcept {
  return 1.;
}

}  // namespace hm3::geometry::point_primitive
