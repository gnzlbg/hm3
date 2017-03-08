#pragma once
/// \file
///
/// Intersection of polylines with points.
#include <hm3/geometry/algorithm/intersection/segment_point.hpp>
#include <hm3/ext/variant.hpp>

namespace hm3::geometry {

namespace intersection_test_polyline_point_detail {

struct intersection_test_polyline_point_fn {
  /// Does the polyline \p s intersect/contain the point \p p?
  template <typename PL, typename P>
  constexpr bool operator()(PL const& pl, P const& p, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Polyline<PL>{});
    static_assert(Point<P>{});
    static_assert(ad_v<P> == ad_v<PL>);
    return ranges::any_of(edges(pl), [&](auto&& e) {
      return intersection_test_segment_point(e, p, abs_tol, rel_tol);
    });
  }
};

}  // namespace intersection_test_polyline_point_detail

namespace {
static constexpr auto const& intersection_test_polyline_point = static_const<
 with_default_tolerance<intersection_test_polyline_point_detail::
                         intersection_test_polyline_point_fn>>::value;
}

namespace intersection_polyline_point_detail {

struct intersection_polyline_point_fn {
  /// Does the polyline \p s intersect/contain the point \p p?
  template <typename PL, typename P>
  constexpr variant<monostate, P> operator()(PL const& pl, P const& p,
                                             num_t abs_tol, num_t rel_tol) const
   noexcept {
    static_assert(Polyline<PL>{});
    static_assert(Point<P>{});
    static_assert(ad_v<P> == ad_v<PL>);
    if (intersection_test_polyline_point(pl, p, abs_tol, rel_tol)) { return p; }
    return {};
  }
};

}  // namespace intersection_polyline_point_detail

namespace {
static constexpr auto const& intersection_polyline_point
 = static_const<with_default_tolerance<
  intersection_polyline_point_detail::intersection_polyline_point_fn>>::value;
}

}  // namespace hm3::geometry
