#pragma once
/// \file
///
/// Box vertices
#include <hm3/geometry/aabb/size.hpp>
#include <hm3/geometry/aabb/vertices.hpp>
#include <hm3/geometry/box/box.hpp>
#include <hm3/geometry/box/centroid.hpp>
#include <hm3/geometry/box/length.hpp>

namespace hm3 {
namespace geometry {

/// Vertex \p v of the Box \p s
template <dim_t Nd>  //
constexpr point<Nd> vertex(box<Nd> const& s, suint_t v) noexcept {
  const auto length_     = length(s);
  const auto half_length = 0.5 * length_;
  const auto x_c         = centroid(s);
  const auto x_p         = aabb_detail::relative_vertex_position<Nd>(v);
  return point<Nd>{x_c().array() + half_length * x_p().array()};
}

/// Box vertices
template <dim_t Nd>  //
constexpr auto vertices(box<Nd> const& s) noexcept {
  constexpr suint_t nvxs = vertex_size(box<Nd>{});
  array<point<Nd>, nvxs> vxs;
  const auto l           = length(s);
  const auto half_length = 0.5 * l;
  const auto x_c         = centroid(s);
  for (suint_t c = 0; c < nvxs; ++c) {
    const auto x_p = aabb_detail::relative_vertex_position<Nd>(c);
    vxs[c]         = x_c().array() + half_length * x_p().array();
  }
  return vxs;
}

}  // namespace geometry
}  // namespace hm3
