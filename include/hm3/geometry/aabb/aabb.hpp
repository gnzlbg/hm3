#pragma once
/// \file
///
/// Axis-Aligned Bounding Box
#include <hm3/geometry/dimension/dimensional.hpp>
#include <hm3/geometry/point/point.hpp>
#include <hm3/geometry/point/vector.hpp>

namespace hm3 {
namespace geometry {

/// Axis-Aligned Bounding Box
template <dim_t Nd>  //
struct aabb : dimensional<Nd> {
  using point_t  = point<Nd>;
  using vector_t = vector<Nd>;

  array<point_t, 2> x;

  constexpr aabb()            = default;
  constexpr aabb(aabb const&) = default;
  constexpr aabb(aabb&&)      = default;
  constexpr aabb& operator=(aabb const&) = default;
  constexpr aabb& operator=(aabb&&) = default;

  /// AABB from \p x_min and \p x_max.
  constexpr aabb(point_t const& x_min, point_t const& x_max)
   : x{{x_min, x_max}} {
    HM3_ASSERT(x[0] != x[1], "aabb min/max bounds are equal: {}", x[0]);
  }

  /// AABB from centroid and lengths
  static constexpr aabb<Nd> at(point_t const& x_c, vector_t const& lengths) {
    for (dim_t d = 0; d < Nd; ++d) {
      HM3_ASSERT(lengths(d) > 0., "aabb length is {} !> 0!", lengths(d));
    }
    return aabb<Nd>(point_t{x_c() - .5 * lengths()},
                    point_t{x_c() + .5 * lengths()});
  }
};

template <dim_t Nd>
constexpr bool operator==(aabb<Nd> const& l, aabb<Nd> const& r) noexcept {
  return l.x[0] == r.x[0] and l.x[1] == r.x[1];
}

template <dim_t Nd>
constexpr bool operator!=(aabb<Nd> const& l, aabb<Nd> const& r) noexcept {
  return !(l == r);
}

}  // namespace geometry
}  // namespace hm3
