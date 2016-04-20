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

template <uint_t Nd> struct sphere_t : dimensional<Nd> {
  point<Nd> centroid;
  num_t radius;
  constexpr sphere_t() = default;
  constexpr sphere_t(point<Nd> centroid_, const num_t radius_) noexcept
   : centroid(std::move(centroid_)), radius(radius_) {}
  num_t operator()(point<Nd> const& x) const noexcept {
    return sphere(x, centroid, radius);
  }
};

///@}  Fixed sphere

/// \name Time-dependent rigid sphere
///@{

template <uint_t Nd> struct moving_sphere : dimensional<Nd> {
  const num_t radius;
  point<Nd> centroid;
  constexpr moving_sphere(const num_t radius_) : radius{radius_} {}
  inline num_t operator()(point<Nd> const& x) const noexcept {
    return sphere(x, centroid, radius);
  }
};

///@} Time-dependent rigid sphere

}  // namespace sd
}  // namespace geometry
}  // namespace hm3
