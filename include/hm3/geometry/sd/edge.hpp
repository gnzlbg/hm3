#pragma once
/// \file
///
/// Implicit signed-distance functions for edges
#include <hm3/geometry/dimension/dimensional.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/vector.hpp>

namespace hm3 {
namespace geometry {
namespace sd {

/// Signed distance function for an edge at point \p p with normal vector \p
/// n. The signed distance is positive in positive normal direction.
template <uint_t Nd>
constexpr num_t edge(point<Nd> x, point<Nd> p, vector<Nd> n) noexcept {
  return n().dot(x() - p());
}

/// Oriented edge at point centroid with normal vector. Signed distance is
/// positive in positive normal direction.
template <int_t Nd> struct fixed_edge : dimensional<Nd> {
  point<Nd> centroid;
  vector<Nd> normal;

  constexpr fixed_edge() = default;

  constexpr fixed_edge(point<Nd> centroid_, vector<Nd> normal_) noexcept
   : centroid(std::move(centroid_)),
     normal(std::move(normal_)) {}

  inline num_t operator()(point<Nd> const& x) const noexcept {
    return edge(x, centroid, normal);
  }
};

}  // namespace sd
}  // namespace geometry
}  // namespace hm3