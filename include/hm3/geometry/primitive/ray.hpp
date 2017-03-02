#pragma once
/// \file
///
/// Ray.
#include <hm3/geometry/primitive/line.hpp>

namespace hm3::geometry {

/// Ray primitive.
namespace ray_primitive {

/// Ray (unbounded)
///
/// \tparam Ad Number of spatial dimensions.
template <dim_t Ad>
struct ray : line<Ad> {
  using geometry_type = trait::ray<Ad>;
  using line_type     = line<Ad>;
  using line<Ad>::line;
  using line<Ad>::operator=;
  using line<Ad>::origin;
  using line<Ad>::x;

  /// Unbounded line from the ray.
  line_type line() const noexcept {
    return static_cast<line_type const&>(*this);
  }

  /// Ray through point \p p in Cartesian direction \p d.
  static ray unit(point<Ad> p, dim_t d) noexcept {
    return ray{p, vec<Ad>::unit(d)};
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/// Is the representation of the rays \p a and \p b equal?
template <dim_t Ad>
bool operator==(ray<Ad> const& a, ray<Ad> const& b) noexcept {
  return static_cast<line<Ad> const&>(a) == static_cast<line<Ad> const&>(b);
}

template <dim_t Ad>
bool operator!=(ray<Ad> const& a, ray<Ad> const& b) noexcept {
  return !(a == b);
}

}  // namespace ray_primitive

using ray_primitive::ray;

}  // namespace hm3::geometry
