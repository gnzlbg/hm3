#pragma once
/// \file
///
/// AABB vertices
#include <hm3/geometry/aabb/aabb.hpp>
#include <hm3/geometry/aabb/concept.hpp>
#include <hm3/geometry/aabb/relative_vertex_position.hpp>
#include <hm3/geometry/aabb/size.hpp>
#include <hm3/geometry/dimension/dimension.hpp>
#include <hm3/geometry/point/point.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/config/fatal_error.hpp>

namespace hm3 {
namespace geometry {

namespace aabb_primitive {

template <typename Shape, CONCEPT_REQUIRES_(AABB<Shape>{})>
constexpr auto vertex_indices(Shape const&) noexcept {
  using idx_t = decltype(vertex_size(Shape{}));
  return view::iota(idx_t{0}, vertex_size(Shape{}));
}

/// Vertex \p v of the AABB \p s
template <dim_t Nd>  //
constexpr point<Nd> vertex(aabb<Nd> const& s, suint_t v) noexcept {
  const auto lengths_     = lengths(s);
  const auto half_lengths = 0.5 * lengths_();
  const auto x_c          = centroid(s);
  const auto x_p          = aabb_detail::relative_vertex_position<Nd>(v);
  return point<Nd>{x_c().array() + half_lengths.array() * x_p().array()};
}

/// AABB vertices
template <dim_t Nd>  //
constexpr auto vertices(aabb<Nd> const& s) noexcept {
  constexpr auto nvxs = vertex_size(aabb<Nd>{});
  array<point<Nd>, nvxs> vxs;
  auto ls                 = lengths(s);
  const auto half_lengths = 0.5 * ls();
  const auto x_c          = centroid(s);
  for (suint_t c = 0; c < nvxs; ++c) {
    const auto x_p = aabb_detail::relative_vertex_position<Nd>(c);
    vxs[c]         = x_c().array() + half_lengths.array() * x_p().array();
  }
  return vxs;
}

}  // namespace aabb_primitive

}  // namespace geometry
}  // namespace hm3
