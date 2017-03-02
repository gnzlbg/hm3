#pragma once
/// \file
///
/// Are two boxs approximately equal
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/algorithm/bounding_length/box.hpp>
#include <hm3/geometry/algorithm/centroid/box.hpp>

namespace hm3::geometry {

namespace approx_box_detail {

struct approx_box_fn {
  /// Computes if the boxes \p a and \p b are approximately equal using the
  /// absolute and relative tolerances \p `abs_tol` and \p `rel_tol`,
  /// respectively.
  template <typename T, typename NT>
  constexpr bool operator()(T const& a, T const& b, NT abs_tol,
                            NT rel_tol) const noexcept {
    static_assert(Same<associated::t_<T>, trait::box<ad_v<T>>>{});
    return approx_point(centroid_box(a), centroid_box(b), abs_tol, rel_tol)
           and approx_number(bounding_length_box(a, 0),
                             bounding_length_box(b, 0), abs_tol, rel_tol);
  }
};

}  // namespace direction_detail

namespace {
static constexpr auto const& approx_box = static_const<
 with_default_tolerance<approx_box_detail::approx_box_fn>>::value;

}  // namespace

}  // namespace hm3::geometry
