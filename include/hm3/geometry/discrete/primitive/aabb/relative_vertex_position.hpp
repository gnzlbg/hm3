#pragma once
/// \file
///
/// AABB relative vertex position.
#include <hm3/geometry/discrete/primitive/point/point.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/config/fatal_error.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace aabb_primitive {

using vidx_t = dim_t;

/// Relative position of AABB vertices (+-1) wrt to the AABB centroid
///
/// Order: Counter Clow Wise (CCW):
///
///       7         6
///        *-------*
///       /|      /|
///      / |     / |       x(1)   x(2)
///   3 *-------* 2|        ^   ^
///     |  *----|--*        |  /
///     | / 4   | / 5       | /
///     |/      |/          |/
///     *-------*           *------> x(0)
///     0       1
///
constexpr sint_t relative_vertex_position(vidx_t vertex_pos,
                                          dim_t dimension) noexcept {
  /// TODO: table lookup
  switch (dimension) {
    case 0: {
      return ((vertex_pos + 1) / 2 % 2) != 0 ? 1 : -1;
    }
    case 1: {
      return ((vertex_pos / 2) % 2) != 0 ? 1 : -1;
    }
    case 2: {
      return ((vertex_pos / 4) % 4) != 0 ? +1 : -1;
    }
    default: { HM3_FATAL_ERROR("unknown dimension {}", dimension); }
  }
}

template <dim_t Nd, dim_t... Is>
constexpr point<Nd> relative_vertex_position(
 suint_t vertex_pos, std::integer_sequence<dim_t, Is...>) noexcept {
  return point<Nd>{
   static_cast<num_t>(relative_vertex_position(vertex_pos, Is))...};
}

template <dim_t Nd>
constexpr point<Nd> relative_vertex_position(vidx_t vertex_pos) noexcept {
  return relative_vertex_position<Nd>(vertex_pos,
                                      std::make_integer_sequence<dim_t, Nd>{});
}

}  // namespace aabb_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
