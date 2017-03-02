#pragma once
/// \file
///
/// Are two planes parallel?
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/concept/plane.hpp>
#include <hm3/geometry/tolerance.hpp>

namespace hm3::geometry {

namespace parallel_plane_detail {

struct parallel_plane_fn {
  /// Are the planes \p a and \p b parallel?
  template <typename T>
  constexpr auto operator()(T const& a, T const& b, num_t abs_tol,
                            num_t rel_tol) const {
    static_assert(Plane<T>{});
    if
      constexpr(ad_v<T> == 1) { return true; }
    else {
      return approx_vector(a.normal(), b.normal(), abs_tol, rel_tol);
    }
  }
};

}  // namespace parallel_plane_detail

namespace {
static constexpr auto const& parallel_plane = static_const<
 with_default_tolerance<parallel_plane_detail::parallel_plane_fn>>::value;
}

}  // namespace hm3::geometry
