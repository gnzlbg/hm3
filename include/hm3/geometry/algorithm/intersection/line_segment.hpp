#pragma once
/// \file
///
/// Intersection of line with points.
#include <hm3/geometry/algorithm/distance.hpp>
#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry {

namespace intersection_test_line_segment_detail {

/// Returns a segment of the line \p r that is large enough to intersect \p s.
template <typename R, typename S>
constexpr S segment_from_line(R&& r, S const& s) {
  // Compute the maximum distance between the origin and the segment:
  num_t max_distance
   = math::max(distance(r.origin(), s.x(0)), distance(r.origin(), s.x(1)));
  // Transform the line into a segment slightly larger than this distance
  using p_t = associated::point_t<S>;
  p_t x_begin(r.origin()() - r.direction()() * max_distance * 1.1);
  p_t x_end(r.origin()() + r.direction()() * max_distance * 1.1);
  S s2 = S(x_begin, x_end);
  // This segment is now large enough to intersect the original segment:
  return s2;
}

struct intersection_test_line_segment_fn {
  /// Does the line \p r intersect the segment \p s?
  template <typename R, typename S>
  constexpr bool operator()(R const& r, S const& s, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Line<R>{});
    static_assert(Segment<S>{});
    static_assert(ad_v<R> == ad_v<S>);

    auto s2 = segment_from_line(r, s);
    return intersection_test_segment_segment(s2, s, abs_tol, rel_tol);
  }
};

}  // namespace intersection_test_line_segment_detail

namespace {
static constexpr auto const& intersection_test_line_segment
 = static_const<with_default_tolerance<
  intersection_test_line_segment_detail::intersection_test_line_segment_fn>>::
  value;
}  // namespace

namespace intersection_line_segment_detail {

struct intersection_line_segment_fn {
  /// Intersection of the line \p r with the segment \p s
  template <typename R, typename S>
  constexpr variant<monostate, S, associated::point_t<S>> operator()(
   R const& r, S const& s, num_t abs_tol, num_t rel_tol) const noexcept {
    static_assert(Line<R>{});
    static_assert(Segment<S>{});
    static_assert(ad_v<R> == ad_v<S>);

    auto s2 = intersection_test_line_segment_detail::segment_from_line(r, s);
    return intersection_segment_segment(s2, s, abs_tol, rel_tol);
  }
};

}  // namespace intersection_line_segment_detail

namespace {
static constexpr auto const& intersection_line_segment
 = static_const<with_default_tolerance<
  intersection_line_segment_detail::intersection_line_segment_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
