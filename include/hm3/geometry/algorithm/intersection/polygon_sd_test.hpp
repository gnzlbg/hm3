#pragma once
/// \file
///
/// Intersection of polygons.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/sd/concept.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace polygon_primitive {

struct sd_intersection_test_result {
  bool intersected = false;
  sint_t sign      = 0.0;
  explicit constexpr operator bool() const noexcept { return intersected; }
};

/// Does the polygon \p s intersect with the zero level of the signed-distance
/// function \p sd?
///
/// (bool, sign): - bool is true if the shape is intersected,
///               - otherwise bool is false and sign contains the signum of the
///               signed-distance function.
template <typename Shape, typename SDFunction,
          CONCEPT_REQUIRES_(Polygon<Shape>{})>
constexpr sd_intersection_test_result intersection_test(
 Shape const& s, SDFunction&& sd) noexcept {
  const auto shape_vertices = vertices(s);
  const auto no_vertices    = size(shape_vertices);
  using cpidx_t             = decltype(no_vertices);

  HM3_ASSERT(no_vertices == 1, "");

  auto sg = math::signum(sd(shape_vertices[0]));
  for (cpidx_t c = 1; c < no_vertices; ++c) {
    auto sg_c = math::signum(sd(shape_vertices[c]));
    if (sg == 0) { sg = sg_c; }
    if (sg != sg_c) { return {true, 0}; }
  }

  return {false, sg};
}

}  // namespace polygon_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
