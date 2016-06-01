#pragma once
/// \file
///
/// Box sizes
#include <hm3/geometry/aabb/size.hpp>
#include <hm3/geometry/box/box.hpp>
#include <hm3/utility/math.hpp>
namespace hm3 {
namespace geometry {

/// Number of vertices
template <dim_t Nd>  //
constexpr auto vertex_size(box<Nd> const&) noexcept {
  return vertex_size(aabb<Nd>{});
}

/// Number of m-dimensional edges
template <dim_t Nd>  //
constexpr auto face_size(box<Nd> const&, dim_t m) noexcept -> dim_t {
  return face_size(aabb<Nd>{}, m);
}

/// Is the box empty? (never)
template <dim_t Nd>  //
constexpr bool empty(box<Nd> const&) {
  return false;
}

}  // namespace geometry
}  // namespace hm3
