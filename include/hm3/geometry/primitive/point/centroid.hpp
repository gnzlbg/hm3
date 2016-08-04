#pragma once
/// \file
///
/// Centroid of a point.
#include <hm3/geometry/algorithm/centroid.hpp>
#include <hm3/geometry/primitive/point/point.hpp>

namespace hm3::geometry::point_primitive {

/// Centroid of a point
template <typename P, typename UP = uncvref_t<P>,
          CONCEPT_REQUIRES_(Same<UP, point<UP::dimension()>>{})>
constexpr decltype(auto) centroid(P&& p) noexcept {
  return std::forward<P>(p);
}

}  // namespace hm3::geometry::point_primitive
