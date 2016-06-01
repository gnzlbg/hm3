#pragma once
/// \file
///
/// Area of a polygon
#include <hm3/geometry/polygon/concept.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace geometry {

/// Compute the signed area of a two dimensional polygon
template <typename P, CONCEPT_REQUIRES_(PolygonD<P, 2>{})>
constexpr num_t signed_area(P&& polygon) noexcept {
  num_t a         = 0.;
  const auto& ps  = vertices(polygon);
  const ppidx_t e = size(ps) - 1;
  a += ps[0](0) * (ps[1](1) - ps[e](1));
  for (ppidx_t i = 1; i != e; ++i) {
    a += ps[i](0) * (ps[i + 1](1) - ps[i - 1](1));
  }
  a += ps[e](0) * (ps[0](1) - ps[e - 1](1));
  return a / 2.;
}

/// Computes the absolute area of a polygon
template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
constexpr num_t area(P&& polygon) noexcept {
  return std::abs(signed_area(std::forward<P>(polygon)));
}

}  // namespace geometry
}  // namespace hm3
