#pragma once
/// \file
///
/// Bounding volume of a aabb
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/algorithm/bounding_length/aabb.hpp>
#include <hm3/geometry/algorithm/centroid/aabb.hpp>
#include <hm3/geometry/algorithm/centroid/box.hpp>
#include <hm3/geometry/algorithm/x_min_max.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace box_of_aabb_detail {

struct box_of_aabb_fn {
  /// Square bounding box of aabb \p p.
  template <typename P>
  constexpr auto operator()(P&& p) const noexcept {
    static_assert(Same<associated::t_<P>, trait::aabb<ad_v<P>>>{});
    auto max_length = bounding_length_aabb.max(p);
    using box_t     = associated::box_t<P>;
    auto bbox       = box_t(centroid_aabb(p), next_num(max_length, 2));

    HM3_ASSERT(
     centroid_box(bbox) == centroid_aabb(p),
     "the centroid of the bounding box of the aabb does not coincide "
     "with the centroid of the aabb:  box:\n    {}\n    x_c: {}\n  aabb:  "
     "  {}\n    x_c: {}\n",
     bbox, centroid_box(bbox), p, centroid_aabb(p));

    HM3_ASSERT(approx.leq(x_min(bbox), x_min(p))
                and approx.geq(x_max(bbox), x_max(p)),
               "the square bounding box of the aabb does not contain the "
               "aabb\n\n  box: {}\n  aabb: {}\n",
               bbox, p);
    return bbox;
  }
};

}  // namespace box_of_aabb_detail

namespace aabb_of_aabb_detail {

struct aabb_of_aabb_fn {
  /// Axis-aligned bounding aabb of \p p.
  template <typename P>
  constexpr decltype(auto) operator()(P&& p) const noexcept {
    static_assert(Same<associated::t_<P>, trait::aabb<ad_v<P>>>{});
    return std::forward<P>(p);
  }
};

}  // namespace aabb_of_aabb_detail

namespace {
constexpr auto const& aabb_of_aabb
 = static_const<aabb_of_aabb_detail::aabb_of_aabb_fn>::value;
constexpr auto const& box_of_aabb
 = static_const<box_of_aabb_detail::box_of_aabb_fn>::value;
}  // namespace

}  // namespace hm3::geometry
