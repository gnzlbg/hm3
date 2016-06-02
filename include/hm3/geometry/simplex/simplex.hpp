#pragma once
/// \file
///
/// Simplex
#include <hm3/geometry/segment.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/triangle.hpp>

namespace hm3 {
namespace geometry {

/// Simplex: points (1D), segments (2D), triangles(3D)
template <dim_t Nd>  // clang-format off
using simplex
 = std::conditional_t<Nd == 1, point<Nd>,
   std::conditional_t<Nd == 2, segment<Nd>,
   std::conditional_t<Nd == 3, triangle<Nd>,
                               void
                     >>>;
// clang-format on

// TODO: compute normal
// TODO: flip normal
// TODO: center
// TODO: square bbox
// TODO: aabb
// TODO: contains

// struct simplex : dimensional<Nd> {
//   using point_t = geometry::point<Nd>;
//   array<point_t, Nd> vertices;
// };

// template <dim_t Nd, CONCEPT_REQUIRES(Nd == 1)>
// square<Nd> bounding_box(simplex<Nd> s) {
//   return square<Nd>{s.vertices[0], math::eps};
// }

// template <dim_t Nd, CONCEPT_REQUIRES(Nd == 2)>
// square<Nd> bounding_box(simplex<Nd> s) {
//   return square<Nd>{s.vertices[0], math::eps};
// }

}  // namespace geometry
}  // namespace hm3
