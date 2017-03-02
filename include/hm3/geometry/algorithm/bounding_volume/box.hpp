#pragma once
/// \file
///
/// Bounding volume of a box
#include <hm3/geometry/algorithm/x_min_max.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace box_of_box_detail {

struct box_of_box_fn {
  /// Square bounding box of \p p.
  template <typename P>
  constexpr decltype(auto) operator()(P&& p) const noexcept {
    static_assert(Same<associated::t_<P>, trait::box<ad_v<P>>>{});
    return p;
  }
};

}  // namespace box_of_box_detail

namespace aabb_of_box_detail {

struct aabb_of_box_fn {
  /// Axis-aligned bounding box of \p p.
  template <typename P>
  constexpr auto operator()(P&& p) const noexcept {
    static_assert(Same<associated::t_<P>, trait::box<ad_v<P>>>{});
    using aabb_t = associated::aabb_t<P>;
    return aabb_t(x_min(p), x_max(p));
  }
};

}  // namespace aabb_of_box_detail

namespace {
static constexpr auto const& aabb_of_box
 = static_const<aabb_of_box_detail::aabb_of_box_fn>::value;

static constexpr auto const& box_of_box
 = static_const<box_of_box_detail::box_of_box_fn>::value;
}

}  // namespace hm3::geometry
