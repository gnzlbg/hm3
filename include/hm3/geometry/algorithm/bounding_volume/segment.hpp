#pragma once
/// \file
///
/// Bounding volume of a segment
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/algorithm/centroid/segment.hpp>
#include <hm3/geometry/concept/segment.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace aabb_of_segment_detail {

struct aabb_of_segment_fn {
  /// AABB of a segment
  template <typename P>
  constexpr decltype(auto) operator()(P&& s) const noexcept {
    static_assert(Segment<uncvref_t<P>>{});
    using aabb_t = associated::aabb_t<P>;
    using p_t    = associated::point_t<aabb_t>;
    p_t xmin(s.x(0)().array().min(s.x(1)().array()));
    p_t xmax(s.x(0)().array().max(s.x(1)().array()));
    for (dim_t d = 0; d < ad_v<P>; ++d) {
      if (not approx_number(xmin(d), xmax(d))) { continue; }
      xmin(d) = prev_num(xmin(d), 2);
      xmax(d) = next_num(xmax(d), 2);
    }
    return aabb_t(xmin, xmax);
  }
};

}  // namespace aabb_of_segment_detail

namespace {
constexpr auto const& aabb_of_segment
 = static_const<aabb_of_segment_detail::aabb_of_segment_fn>::value;
}

namespace box_of_segment_detail {

struct box_of_segment_fn {
  /// Box of a segment
  template <typename P>
  constexpr decltype(auto) operator()(P&& s) const noexcept {
    static_assert(Segment<uncvref_t<P>>{});
    using box_t = associated::box_t<P>;
    return box_t(centroid_segment(s),
                 (s.x(0)() - s.x(1)()).array().abs().maxCoeff());
  }
};

}  // namespace box_of_segment_detail

namespace {
constexpr auto const& box_of_segment
 = static_const<box_of_segment_detail::box_of_segment_fn>::value;
}

}  // namespace hm3::geometry
