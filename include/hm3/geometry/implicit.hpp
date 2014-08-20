#pragma once
/// \file implicit.hpp Implicit geometries using signed-distance functions.
#include <hm3/geometry/point.hpp>

namespace hm3 {
namespace geometry {

/// Implicit signed-distance functions
///
/// Outside:  g(x) > 0 outside,
/// Surface: g(x) = 0, and
/// Inside:  g(x) < 0 .
namespace implicit {

/// \name Sphere
///@{

/// Signed distance to a sphere
template <int_t nd>
num_t sphere(point<nd> const& x, point<nd> const& x_c, const num_t r) noexcept {
  return (x() - x_c()).norm() - r;
}

///@}  // Sphere

/// \name Fixed Sphere
///@{

template <int_t nd> struct fixed_sphere {
  const point<nd> center;
  const num_t radius;
  constexpr fixed_sphere(point<nd> const& center_, const num_t radius_) noexcept
   : center(center_),
     radius(radius_) {}
  inline num_t operator()(point<nd> const& x) const noexcept {
    return sphere(x, center, radius);
  }
};

/// Dimension of a fixed sphere
template <int_t nd>
constexpr auto dimension(fixed_sphere<nd> const&)
 HM3_RETURNS(std::integral_constant<int_t, nd>{});

///@}  Fixed sphere

/// \name Time-dependent rigid sphere
///@{

template <int_t nd> struct moving_sphere {
  const num_t radius;
  point<nd> center;
  constexpr moving_sphere(const num_t radius_) : radius{radius_} {}
  inline num_t operator()(point<nd> const& x) const noexcept {
    return sphere(x, center, radius);
  }
};

/// Dimension of a moving sphere
template <int_t nd>
constexpr auto dimension(moving_sphere<nd> const&)
 HM3_RETURNS(std::integral_constant<int_t, nd>{});

///@} Time-dependent rigid sphere

/// Oriented:
/// - points in 1D,
/// - lines in 2D, and
/// - planes in 3D.
template <int_t nd> struct edge {
  const point<nd> center;
  const vector<nd> normal;

  constexpr edge(point<nd> const& center_, vector<nd> const& normal_) noexcept
   : center(center_),
     normal(normal_) {}

  inline auto operator()(point<nd> const& x) const noexcept -> num_t {
    return normal().dot(x() - center());
  }
};

/// Dimension of an edge
template <int_t nd>
constexpr auto dimension(edge<nd>)
 HM3_RETURNS(std::integral_constant<int_t, nd>{});

/// Inverts a signed distance field
template <class SignedDistanceF> auto invert(SignedDistanceF& f) {
  return [&](point<dimension(f)> const& x) { return -f(x); };
}

/// Union of a signed distance field
template <class SignedDistanceF1, class SignedDistanceF2>
auto take_union(SignedDistanceF1& f1, SignedDistanceF2& f2) {
  static_assert(dimension(f1) == dimension(f2), "dimension mismatch");
  return [&](point<dimension(f1)> const& x) { return std::min(f1(x), f2(x)); };
}

/// Intersection of a signed distance field
template <class SignedDistanceF1, class SignedDistanceF2>
auto take_intersection(SignedDistanceF1& f1, SignedDistanceF2& f2) {
  constexpr int_t nd1 = decltype(dimension(f1))();
  constexpr int_t nd2 = decltype(dimension(f2))();
  static_assert(nd1 == nd2, "dimension mismatch");
  return [&](point<nd1> const& x) { return std::max(f1(x), f2(x)); };
}

/// Difference of a signed distance field
template <class SignedDistanceF1, class SignedDistanceF2>
auto take_difference(SignedDistanceF1& f1, SignedDistanceF2& f2) {
  static_assert(dimension(f1) == dimension(f2), "dimension mismatch");
  return [&](point<dimension(f1)> const& x) { return std::max(f1(x), -f2(x)); };
}

}  // namespace implicit
}  // namespace geometry
}  // namespace hm3
