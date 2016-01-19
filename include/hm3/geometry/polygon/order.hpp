#pragma once
/// \file
///
/// Verifies the order of the polygon points
///
/// TODO: this assumes that the points are "in some order" (either cw or ccw).
#include <hm3/geometry/polygon/concept.hpp>

namespace hm3 {
namespace geometry {

template <typename P, CONCEPT_REQUIRES_(PolygonD<P, 2>{})>
constexpr bool counter_clock_wise(P&& polygon) {
  HM3_ASSERT(!polygon.empty(), "polygon is empty");
  const auto&& ps = corners(polygon);
  const auto e    = size(ps) - 1;
  num_t tmp       = 0.;
  for (suint_t i = 0; i != e; ++i) {
    tmp += (ps[i + 1](0) - ps[i](0)) * (ps[i + 1](1) + ps[i](1));
  }
  {  // wrap around
    tmp += (ps[0](0) - ps[e](0)) * (ps[0](1) + ps[e](1));
  }
  return tmp < 0;
}

template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
constexpr bool clock_wise(P&& polygon) {
  return !counter_clock_wise(std::forward<P>(polygon));
}

}  // namespace geometry
}  // namespace hm3
