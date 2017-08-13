#pragma once
/// \file
///
/// Implicit signed-distance functions for spheres
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/sd/transformations/translation.hpp>

namespace hm3::geometry::sd {

namespace sphere_detail {

/// \name Sphere
///@{

struct sphere_fn {
  /// Signed distance to a sphere centered at zero
  template <typename P>
  constexpr num_t operator()(P x, const num_t r) const noexcept {
    return x.norm() - r;
  }
};

}  // namespace sphere_detail

namespace {
constexpr auto const& sphere = static_const<sphere_detail::sphere_fn>::value;
}

///@}  // Sphere

template <dim_t Ad>
struct sphere_at_origin : signed_distance<Ad> {
  num_t radius_                = 1.0;
  constexpr sphere_at_origin() = default;
  constexpr sphere_at_origin(num_t radius) noexcept : radius_(radius) {
    HM3_ASSERT(radius_ > 0., "sphere radius = {} !> 0.", radius_);
  }

  void set_radius(num_t r) noexcept {
    HM3_ASSERT(r > 0., "sphere radius = {} !> 0.", r);
    radius_ = r;
  }

  template <typename P>
  constexpr num_t operator()(P const& x) const noexcept {
    return invoke_unwrapped(sphere, x, radius_);
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <dim_t Ad>
using sphere_at_point = translation<sphere_at_origin<Ad>>;

}  // namespace hm3::geometry::sd
