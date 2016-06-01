#pragma once
/// \file
///
/// Centroid of a ccw-sorted polygon
#include <hm3/geometry/dimensions.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/polygon/concept.hpp>
#include <hm3/geometry/polygon/order.hpp>

namespace hm3 {
namespace geometry {

template <typename P, CONCEPT_REQUIRES_(PolygonD<P, 2>{})>
constexpr auto centroid(P&& polygon) noexcept -> point<dimension_t<P>{}> {
  HM3_ASSERT(counter_clock_wise(polygon), "not sorted counter clock wise!");
  auto x       = point<dimension_t<P>{}>::constant(0.);
  auto a       = signed_area(polygon);
  auto&& ps    = vertices(polygon);
  const auto e = size(ps) - 1;

  auto kernel = [&](auto&& l, auto&& r) {
    return (ps[l](0) * ps[r](1) - ps[r](0) * ps[l](1)) * (ps[l]() + ps[r]());
  };

  for (ppidx_t i = 0; i < e; ++i) { x() += kernel(i, i + 1); }
  {  // wrap around
    x() += kernel(e, 0);
  }

  x() /= (6. * a);
  return x;
}

}  // namespace geometry
}  // namespace hm3
