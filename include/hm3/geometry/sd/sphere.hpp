#pragma once
/// \file
///
/// Implicit signed-distance functions for spheres
#include <hm3/geometry/point.hpp>

namespace hm3 {
namespace geometry {
namespace sd {

/// \name Sphere
///@{

/// Signed distance to a sphere centered at zero
template <uint_t Nd> num_t sphere(point<Nd> x, const num_t r) noexcept {
  return x().norm() - r;
}

/// Signed distance to a sphere
template <uint_t Nd>
num_t sphere(point<Nd> const& x, point<Nd> const& x_c, const num_t r) noexcept {
  return sphere(point<Nd>(x() - x_c()), r);
}

///@}  // Sphere

/// \name Fixed Sphere
///@{

template <uint_t Nd> struct fixed_sphere {
  const point<Nd> centroid;
  const num_t radius;
  constexpr fixed_sphere(point<Nd> centroid_, const num_t radius_) noexcept
   : centroid(std::move(centroid_)),
     radius(radius_) {}
  inline num_t operator()(point<Nd> const& x) const noexcept {
    return sphere(x, centroid, radius);
  }
};

/// Dimension of a fixed sphere
template <uint_t Nd>
constexpr auto dimension(fixed_sphere<Nd> const&) noexcept {
  return std::integral_constant<int_t, Nd>{};
}

///@}  Fixed sphere

/// \name Time-dependent rigid sphere
///@{

template <uint_t Nd> struct moving_sphere {
  const num_t radius;
  point<Nd> centroid;
  constexpr moving_sphere(const num_t radius_) : radius{radius_} {}
  inline num_t operator()(point<Nd> const& x) const noexcept {
    return sphere(x, centroid, radius);
  }
};

/// Dimension of a moving sphere
template <uint_t Nd> constexpr auto dimension(moving_sphere<Nd> const&) {
  return std::integral_constant<int_t, Nd>{};
}

///@} Time-dependent rigid sphere

}  // namespace sd
}  // namespace geometry
}  // namespace hm3
