#pragma once
/// \file
///
/// Implicit geometries using signed-distance functions.
#include <hm3/geometry/point.hpp>

namespace hm3 {
namespace geometry {

/// Implicit signed-distance functions
///
/// Outside:  g(x) > 0 outside,
/// Surface: g(x) = 0, and
/// Inside:  g(x) < 0 .
namespace sd {

/// \name Sphere
///@{

/// Signed distance to a sphere
template <int_t Nd>
num_t sphere(point<Nd> const& x, point<Nd> const& x_c, const num_t r) noexcept {
  return (x() - x_c()).norm() - r;
}

///@}  // Sphere

/// \name Fixed Sphere
///@{

template <int_t Nd> struct fixed_sphere {
  const point<Nd> center;
  const num_t radius;
  constexpr fixed_sphere(point<Nd> center_, const num_t radius_) noexcept
   : center(std::move(center_)),
     radius(radius_) {}
  inline num_t operator()(point<Nd> const& x) const noexcept {
    return sphere(x, center, radius);
  }
};

/// Dimension of a fixed sphere
template <int_t Nd> constexpr auto dimension(fixed_sphere<Nd> const&) noexcept {
  return std::integral_constant<int_t, Nd>{};
}

///@}  Fixed sphere

/// \name Time-dependent rigid sphere
///@{

template <int_t Nd> struct moving_sphere {
  const num_t radius;
  point<Nd> center;
  constexpr moving_sphere(const num_t radius_) : radius{radius_} {}
  inline num_t operator()(point<Nd> const& x) const noexcept {
    return sphere(x, center, radius);
  }
};

/// Dimension of a moving sphere
template <int_t Nd> constexpr auto dimension(moving_sphere<Nd> const&) {
  return std::integral_constant<int_t, Nd>{};
}

///@} Time-dependent rigid sphere

/// Oriented:
/// - points in 1D,
/// - lines in 2D, and
/// - planes in 3D.
template <int_t Nd> struct edge {
  const point<Nd> center;
  const vector<Nd> normal;

  constexpr edge(point<Nd> center_, vector<Nd> normal_) noexcept
   : center(std::move(center_)),
     normal(std::move(normal_)) {}

  inline auto operator()(point<Nd> const& x) const noexcept -> num_t {
    return normal().dot(x() - center());
  }
};

/// Dimension of an edge
template <int_t Nd> constexpr auto dimension(edge<Nd>) noexcept {
  return std::integral_constant<int_t, Nd>{};
}

/// Inverts a signed distance field
template <typename SignedDistanceF> auto invert(SignedDistanceF& f) {
  return [&](auto&& x) { return -f(x); };
}

/// Union of a signed distance field
template <typename SignedDistanceF1, typename SignedDistanceF2>
auto take_union(SignedDistanceF1&& f1, SignedDistanceF2&& f2) {
  return [=](auto&& x) { return std::min(f1(x), f2(x)); };
}

/// Union of a signed distance field
template <typename SignedDistanceF1, typename... SignedDistanceFs>
auto take_union(SignedDistanceF1&& f1, SignedDistanceFs&&... fs) {
  return take_union(f1, take_union(fs...));
}

/// Intersection of a signed distance field
template <typename SignedDistanceF1, typename SignedDistanceF2>
auto take_intersection(SignedDistanceF1& f1, SignedDistanceF2& f2) {
  return [&](auto&& x) { return std::max(f1(x), f2(x)); };
}

/// Difference of a signed distance field
template <typename SignedDistanceF1, typename SignedDistanceF2>
auto take_difference(SignedDistanceF1& f1, SignedDistanceF2& f2) {
  return [&](auto&& x) { return std::max(f1(x), -f2(x)); };
}

}  // namespace sd
}  // namespace geometry
}  // namespace hm3
