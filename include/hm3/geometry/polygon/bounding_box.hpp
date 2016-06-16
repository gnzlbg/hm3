#pragma once
/// \file
///
/// Bounding box of a polygon
#include <hm3/geometry/aabb/aabb.hpp>
#include <hm3/geometry/aabb/bounds.hpp>
#include <hm3/geometry/aabb/length.hpp>
#include <hm3/geometry/box/box.hpp>
#include <hm3/geometry/polygon/concept.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace geometry {

/// Bounds of a polygon
template <typename P, dim_t Nd = uncvref_t<P>::dimension(),
          CONCEPT_REQUIRES_(PolygonD<P, Nd>{})>
constexpr aabb_bounds<Nd> bounds(P&& polygon) noexcept {
  aabb_bounds<Nd> b;
  const auto& vs = vertices(polygon);
  for (auto&& v : vs) {
    for (dim_t d = 0; d < Nd; ++d) {
      b.min(d) = std::min(b.min(d), v(d));
      b.max(d) = std::max(b.max(d), v(d));
    }
  }
  {
    // polygon can lie on a Cartesian plane such that the length of the AABB
    // normal to the plane is zero. The fix is to add some eps:
    for (dim_t d = 0; d < Nd; ++d) {
      if (math::approx(b.min(d), b.max(d))) {
        b.min(d) -= math::eps;
        b.max(d) += math::eps;
      }
    }
  }
  HM3_ASSERT(b.valid(), "invalid aabb bounds {} of polygon {}", b, polygon);

  return b;
}

/// Axis-Aligned Bounding Box of a polygon
template <typename P, dim_t Nd = uncvref_t<P>::dimension(),
          CONCEPT_REQUIRES_(PolygonD<P, Nd>{})>
constexpr aabb<Nd> axis_aligned_bounding_box(P&& polygon) noexcept {
  auto b = bounds(polygon);
  HM3_ASSERT(b.valid(), "invalid aabb bounds {} of polygon {}", b, polygon);
  return aabb<Nd>{b.min, b.max};
}

/// Square Bounding Box of a polygon
template <typename P, dim_t Nd = uncvref_t<P>::dimension(),
          CONCEPT_REQUIRES_(PolygonD<P, Nd>{})>
constexpr box<Nd> square_bounding_box(P&& polygon) noexcept {
  auto abb = axis_aligned_bounding_box(polygon);
  return box<Nd>{centroid(abb), max_length(abb)};
}

}  // namespace geometry
}  // namespace hm3
