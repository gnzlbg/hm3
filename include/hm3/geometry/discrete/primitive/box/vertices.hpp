#pragma once
/// \file
///
/// Access to vertices of a box.
#include <hm3/geometry/discrete/access/vertex.hpp>
#include <hm3/geometry/discrete/primitive/aabb/vertices.hpp>
#include <hm3/geometry/discrete/primitive/box/bounding_length.hpp>
#include <hm3/geometry/discrete/primitive/box/box.hpp>
#include <hm3/geometry/discrete/primitive/box/centroid.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace geometry {

namespace discrete {
namespace box_primitive {

/// Number of vertices in an box.
template <dim_t Nd>  //
constexpr dim_t vertex_size(box<Nd> const&) noexcept {
  return aabb_primitive::vertex_size(Nd);
}

/// Number of m-dimensional edges
template <dim_t Nd>  //
constexpr dim_t face_size(box<Nd> const&, dim_t m) noexcept {
  return aabb_primitive::face_size(Nd, m);
}

/// Vertex \p v of the box \p s.
template <dim_t Nd>  //
constexpr point<Nd> vertex(box<Nd> const& s, dim_t v) noexcept {
  HM3_ASSERT(v < vertex_size(s), "");
  const auto x_p          = aabb_primitive::relative_vertex_position<Nd>(v);
  const num_t half_length = 0.5 * s.length_;
  return point<Nd>{s.centroid_().array() + half_length * x_p().array()};
}

/// Vertices of the box \p s.
template <dim_t Nd>  //
constexpr auto vertices(box<Nd> const& s) noexcept {
  constexpr auto nvxs = vertex_size(box<Nd>{});
  array<point<Nd>, nvxs> vxs;
  const auto half_length = 0.5 * s.length_;
  for (suint_t c = 0; c < nvxs; ++c) {
    const auto x_p = aabb_primitive::relative_vertex_position<Nd>(c);
    vxs[c]         = s.centroid_().array() + half_length * x_p().array();
  }
  return vxs;
}

}  // namespace box_primitive
}  // namespace discrete

}  // namespace geometry
}  // namespace hm3
