#pragma once
/// \file
///
/// Access to vertices of an AABB
#include <hm3/geometry/access/vertex.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
#include <hm3/geometry/primitive/aabb/bounding_length.hpp>
#include <hm3/geometry/primitive/aabb/centroid.hpp>
#include <hm3/geometry/primitive/aabb/relative_vertex_position.hpp>
#include <hm3/utility/math.hpp>

namespace hm3::geometry::aabb_primitive {

/// Number of vertices in an \p nd dimensional AABB.
constexpr dim_t vertex_size(dim_t nd) noexcept {
  return math::ipow(dim_t{2}, nd);
}

/// Number of vertices in an AABB.
template <dim_t Nd>
constexpr dim_t vertex_size(aabb<Nd> const&) noexcept {
  return vertex_size(Nd);
}

/// Number of m-dimensional edges in an \p nd dimensional AABB.
constexpr dim_t face_size(dim_t nd, dim_t m) noexcept {
  return m <= nd
          ? math::ipow(dim_t{2}, dim_t(nd - m))
             * math::binomial_coefficient(nd, m)
          : dim_t{0};
}

/// Number of m-dimensional edges
template <dim_t Nd>
constexpr dim_t face_size(aabb<Nd> const&, dim_t m) noexcept {
  return face_size(Nd, m);
}

/// Vertex \p v of the AABB \p s.
template <dim_t Nd>
constexpr point<Nd> vertex(aabb<Nd> const& s, dim_t v) noexcept {
  HM3_ASSERT(v < vertex_size(s), "");
  const auto lengths_     = all_bounding_lengths(s);
  const auto half_lengths = 0.5 * lengths_();
  const auto x_c          = centroid(s);
  const auto x_p          = relative_vertex_position<Nd>(v);
  return point<Nd>{x_c().array() + half_lengths.array() * x_p().array()};
}

/// Vertices of the AABB \p s.
template <dim_t Nd>
constexpr auto vertices(aabb<Nd> const& s) noexcept {
  constexpr auto nvxs = vertex_size(aabb<Nd>{});
  array<point<Nd>, nvxs> vxs;
  auto ls                 = all_bounding_lengths(s);
  const auto half_lengths = 0.5 * ls();
  const auto x_c          = centroid(s);
  for (suint_t c = 0; c < nvxs; ++c) {
    const auto x_p = relative_vertex_position<Nd>(c);
    vxs[c]         = x_c().array() + half_lengths.array() * x_p().array();
  }
  return vxs;
}

}  // namespace hm3::geometry::aabb_primitive
