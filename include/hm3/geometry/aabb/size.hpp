#pragma once
/// \file
///
/// AABB sizes
#include <hm3/geometry/aabb/concept.hpp>
#include <hm3/utility/math.hpp>
namespace hm3 {
namespace geometry {

/// Number of vertices
template <typename Shape, CONCEPT_REQUIRES_(AABB<Shape>{})>
constexpr auto vertex_size(Shape const&) noexcept {
  constexpr dim_t nd = dimension(Shape{});
  return math::ipow(dim_t{2}, nd);
}

/// Number of m-dimensional edges
template <typename Shape, CONCEPT_REQUIRES_(AABB<Shape>{})>
constexpr auto face_size(Shape const&, dim_t m) noexcept -> dim_t {
  constexpr dim_t nd = dimension(Shape{});
  return m <= nd
          ? math::ipow(dim_t{2}, dim_t(nd - m))
             * math::binomial_coefficient(nd, m)
          : dim_t{0};
}

/// Is an AABB empty? (i.e. does it contain zero vertices?)
template <typename Shape, CONCEPT_REQUIRES_(AABB<Shape>{})>
constexpr bool empty(Shape const&) {
  return false;
}

}  // namespace geometry
}  // namespace hm3
