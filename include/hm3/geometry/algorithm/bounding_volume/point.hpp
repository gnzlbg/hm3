#pragma once
/// \file
///
/// Bounding volume of a point
#include <hm3/geometry/algorithm/bounding_volume/box.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace box_of_point_detail {

struct box_of_point_fn {
  /// Square Bounding Box of point \p p.
  template <typename P>
  constexpr auto operator()(P&& p) const noexcept {
    static_assert(Point<uncvref_t<P>>{});
    using box_t = associated::box_t<P>;
    return box_t{p, 2. * math::eps};
  }
};

}  // namespace box_of_point_detail

namespace {
static constexpr auto const& box_of_point
 = static_const<box_of_point_detail::box_of_point_fn>::value;
}

namespace aabb_of_point_detail {

struct aabb_of_point_fn {
  /// Axis-Aligned Bounding Box of point \p p (it is just a box).
  template <typename P>
  constexpr auto operator()(P&& p) const noexcept {
    static_assert(Point<uncvref_t<P>>{});
    return aabb_of_box(box_of_point(std::forward<P>(p)));
  }
};

}  // namespace aabb_of_point_detail

namespace {
static constexpr auto const& aabb_of_point
 = static_const<aabb_of_point_detail::aabb_of_point_fn>::value;
}

}  // namespace hm3::geometry
