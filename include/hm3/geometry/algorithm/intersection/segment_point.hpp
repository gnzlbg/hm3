#pragma once
/// \file
///
/// Intersection of line segments with points.
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/algorithm/line_intersection_parameter/segment_point.hpp>
#include <hm3/io/ascii.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry {

namespace intersection_test_segment_point_detail {

struct intersection_test_segment_point_fn {
  /// Does the segment \p s intersect/contain the point \p p?
  template <typename T, typename U>
  constexpr bool operator()(T const& s, U const& p, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Segment<T>{});
    static_assert(Point<U>{});
    static_assert(ad_v<T> == ad_v<U>);

    /// Compute the line parameter of the point
    const auto t_p_
     = intersection_parameter_segment_point(s, p, on_segment, abs_tol, rel_tol);
    if (!t_p_) { return false; }  // point is not on the line => no intersection

    const num_t t_p = t_p_.value();

    // If the parameter is between zero and 1, the point is in the line segment
    if (approx.geq(t_p, 0., abs_tol, rel_tol)
        and approx.leq(t_p, 1., abs_tol, rel_tol)) {
      return true;
    }
    return false;
  }
};

}  // namespace intersection_test_segment_point_detail

namespace {
static constexpr auto const& intersection_test_segment_point
 = static_const<with_default_tolerance<
  intersection_test_segment_point_detail::intersection_test_segment_point_fn>>::
  value;
}

namespace intersection_segment_point_detail {

struct intersection_segment_point_fn {
  /// Intersection of a segment \p s with a point \p p is eitherhe point or
  /// nothing.
  template <typename T, typename U>
  constexpr variant<monostate, U> operator()(T const& s, U const& p,
                                             num_t abs_tol, num_t rel_tol) const
   noexcept {
    static_assert(Segment<T>{});
    static_assert(Point<U>{});
    static_assert(ad_v<T> == ad_v<U>);

    if (intersection_test_segment_point(s, p, abs_tol, rel_tol)) { return p; }
    return {};
  }
};

}  // namespace intersection_segment_point_detail

namespace {
static constexpr auto const& intersection_segment_point
 = static_const<with_default_tolerance<
  intersection_segment_point_detail::intersection_segment_point_fn>>::value;
}

}  // namespace hm3::geometry
