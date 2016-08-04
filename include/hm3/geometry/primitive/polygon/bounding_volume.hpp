#pragma once
/// \file
///
/// Bounding Volumes of a plane polygon.
#include <hm3/geometry/algorithm/bounding_volume.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/polygon/bounding_length.hpp>

namespace hm3::geometry::polygon_primitive {

/// Axis-Aligned Bounding Box of the polygon \p p.
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polygon<UP, Nd>{})>
constexpr aabb<Nd> axis_aligned_bounding_box(P&& p) {
  aabb<Nd> bounds;
  bounds.x[0] = point<Nd>::constant(math::highest<num_t>);
  bounds.x[1] = point<Nd>::constant(math::lowest<num_t>);
  for (auto&& v : vertices(p)) {
    for (dim_t d = 0; d < Nd; ++d) {
      bounds.x[0](d) = std::min(bounds.x[0](d), v(d));
      bounds.x[1](d) = std::max(bounds.x[1](d), v(d));
    }
  }
  // A polygon might lie on a Cartesian plane, such that the length of the AABB
  // along that plane (and hence its volume) is zero.
  // This produces a "tight" AABB with non-zero (but very small) volume:
  for (dim_t d = 0; d < Nd; ++d) {
    if (math::approx(bounds.x[0](d), bounds.x[1](d))) {
      bounds.x[0](d) -= math::eps;
      bounds.x[1](d) += math::eps;
    }
  }
  bounds.assert_valid();
  return bounds;
}

/// Square Bounding Box of the polygon \p s.
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polygon<UP, Nd>{})>
constexpr box<Nd> square_bounding_box(P&& p) {
  auto bounds = axis_aligned_bounding_box(p);
  return box<Nd>{centroid(bounds), max_bounding_length(bounds)};
}

}  // namespace hm3::geometry::polygon_primitive
