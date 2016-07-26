#pragma once
/// \file
///
/// Centroid of a point.
#include <hm3/geometry/discrete/algorithm/centroid.hpp>
#include <hm3/geometry/discrete/primitive/point/point.hpp>

namespace hm3 {
namespace geometry {

namespace discrete {
namespace point_primitive {

/// Centroid of a point
template <typename P, typename UP = uncvref_t<P>,
          CONCEPT_REQUIRES_(Same<UP, point<UP::dimension()>>{})>
constexpr decltype(auto) centroid(P&& p) noexcept {
  return std::forward<P>(p);
}

}  // namespace point_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
