#pragma once
/// \file
///
/// Are two aabbs approximately equal
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/algorithm/x_min_max.hpp>

namespace hm3::geometry {

namespace approx_aabb_detail {

struct approx_aabb_fn {
  /// Computes if the AABBs \p a and \p b are approximately equal using the
  /// absolute and relative tolerances \p `abs_tol` and \p `rel_tol`,
  /// respectively.
  template <typename T, typename NT>
  constexpr bool operator()(T const& a, T const& b, NT abs_tol,
                            NT rel_tol) const noexcept {
    static_assert(Same<associated::t_<T>, trait::aabb<ad_v<T>>>{});
    return approx_point(x_min(a), x_min(b), abs_tol, rel_tol)
           and approx_point(x_max(a), x_max(b), abs_tol, rel_tol);
  }
};

}  // namespace direction_detail

namespace {
static constexpr auto const& approx_aabb = static_const<
 with_default_tolerance<approx_aabb_detail::approx_aabb_fn>>::value;

}  // namespace

}  // namespace hm3::geometry
