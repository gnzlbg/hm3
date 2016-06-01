#pragma once
/// \file
///
/// AABB sizes
#include <hm3/geometry/aabb/aabb.hpp>
#include <hm3/utility/math.hpp>
namespace hm3 {
namespace geometry {

/// Number of vertices
template <dim_t Nd>  //
constexpr auto vertex_size(aabb<Nd> const&) noexcept {
  return math::ipow(dim_t{2}, Nd);
}

/// Number of m-dimensional edges
template <dim_t Nd>  //
constexpr auto face_size(aabb<Nd> const&, dim_t m) noexcept -> dim_t {
  constexpr dim_t nd = Nd;
  return m <= nd
          ? math::ipow(dim_t{2}, dim_t(nd - m))
             * math::binomial_coefficient(nd, m)
          : dim_t{0};
}

/// Is an AABB empty? (i.e. does it contain zero vertices?)
template <dim_t Nd>  //
constexpr bool empty(aabb<Nd> const&) {
  return false;
}

}  // namespace geometry
}  // namespace hm3
