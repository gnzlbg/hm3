#pragma once
/// \file
///
/// Intersection of ray with points.
#include <hm3/ext/variant.hpp>
#include <hm3/geometry/algorithm/distance.hpp>
#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>

namespace hm3::geometry {

namespace intersection_test_ray_segment_detail {

/// Returns a segment of the ray \p r that is large enough to intersect \p s.
template <typename R, typename S>
constexpr S segment_from_ray(R&& r, S const& s) {
  // Compute the maximum distance between the origin and the segment:
  num_t max_distance
   = math::max(distance(r.origin(), s.x(0)), distance(r.origin(), s.x(1)));
  // Transform the ray into a segment slightly larger than this distance
  using p_t = associated::point_t<S>;
  p_t x_end(r.origin()() + r.direction()() * max_distance * 1.1);
  S s2 = S(r.origin(), x_end);
  // This segment is now large enough to intersect the original segment:
  return s2;
}

struct intersection_test_ray_segment_fn {
  /// Does the ray \p r intersect the segment \p s?
  template <typename R, typename S>
  constexpr bool operator()(R const& r, S const& s, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Ray<R>{});
    static_assert(Segment<S>{});
    static_assert(ad_v<R> == ad_v<S>);

    auto s2 = segment_from_ray(r, s);
    return intersection_test_segment_segment(s2, s, abs_tol, rel_tol);
  }
};

}  // namespace intersection_test_ray_segment_detail

namespace {
constexpr auto const& intersection_test_ray_segment
 = static_const<with_default_tolerance<
  intersection_test_ray_segment_detail::intersection_test_ray_segment_fn>>::
  value;
}  // namespace

namespace intersection_ray_segment_detail {

struct intersection_ray_segment_fn {
  /// Intersection of the ray \p r with the segment \p s
  template <typename R, typename S>
  constexpr variant<monostate, S, associated::point_t<S>> operator()(
   R const& r, S const& s, num_t abs_tol, num_t rel_tol) const noexcept {
    static_assert(Ray<R>{});
    static_assert(Segment<S>{});
    static_assert(ad_v<R> == ad_v<S>);

    auto s2 = intersection_test_ray_segment_detail::segment_from_ray(r, s);
    return intersection_segment_segment(s2, s, abs_tol, rel_tol);
  }
};

}  // namespace intersection_ray_segment_detail

namespace {
constexpr auto const& intersection_ray_segment
 = static_const<with_default_tolerance<
  intersection_ray_segment_detail::intersection_ray_segment_fn>>::value;
}  // namespace

namespace intersection_test_non_degenerate_ray_segment_detail {

struct intersection_test_non_degenerate_ray_segment_fn {
  /// Non-degenerate intersection test of the ray \p r with the segment \p s
  template <typename R, typename S>
  constexpr non_degenerate_intersection_test_result operator()(
   R const& r, S const& s, num_t abs_tol, num_t rel_tol) const noexcept {
    static_assert(Ray<R>{});
    static_assert(Segment<S>{});
    static_assert(ad_v<R> == ad_v<S>);

    auto s2     = intersection_test_ray_segment_detail::segment_from_ray(r, s);
    auto result = intersection_segment_segment(s2, s, abs_tol, rel_tol);

    // If the ray intersects the segment on a point, and that point is not one
    // of the segment vertices, the intersection is non-degenerate.
    return visit(
     [&](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if
         constexpr(Point<T>{}) {
           if (not approx_point(s.x(0), v) and not approx_point(s.x(1), v)) {
             return non_degenerate_intersection_test_result::intersection;
           }
           return non_degenerate_intersection_test_result::
            degenerate_intersection;
         }
       else if
         constexpr(Segment<T>{}) {
           return non_degenerate_intersection_test_result::
            degenerate_intersection;
         }
       else {
         static_assert(Same<T, monostate>{});
         return non_degenerate_intersection_test_result::no_intersection;
       }
     },
     result);
  }
};

}  // namespace intersection_test_non_degenerate_ray_segment_detail

namespace {
constexpr auto const& intersection_test_non_degenerate_ray_segment
 = static_const<with_default_tolerance<
  intersection_test_non_degenerate_ray_segment_detail::
   intersection_test_non_degenerate_ray_segment_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
