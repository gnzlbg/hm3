#pragma once
/// \file
///
/// Intersection of line segments with aabbs.
#include <hm3/geometry/algorithm/direction/segment.hpp>
#include <hm3/geometry/algorithm/intersection/ray_aabb.hpp>
#include <hm3/ext/variant.hpp>

namespace hm3::geometry {

namespace intersection_segment_aabb_detail {

struct intersection_segment_aabb_fn {
  /// Intersection of a segment \p s with a aabb \p p is eitherhe aabb or
  /// nothing.
  template <typename S, typename A>
  constexpr variant<monostate, S, associated::point_t<S>> operator()(
   S const& s, A const& a, num_t abs_tol, num_t rel_tol) const noexcept {
    static_assert(Segment<S>{});
    static_assert(AABB<A>{});
    static_assert(ad_v<S> == ad_v<A>);
    using p_t   = associated::point_t<S>;
    using r_t   = variant<monostate, S, p_t>;
    using ray_t = associated::ray_t<S>;

    // Build ray that fully overlaps the segment, that is, such that
    // intersection(segment, ray) == segment:
    auto ray_dir    = direction_segment(s);
    auto ray_origin = s.x(0);
    ray_t r(ray_origin, ray_dir);

    // Compute the intersection between the ray and the aabb
    auto ray_aabb_int = intersection_ray_aabb(r, a, abs_tol, rel_tol);

    return visit(
     [&](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if
         constexpr(Point<T>{}) {
           // If the ray intersects the aabb on a point:
           // - if the intersection point is on the segment, return the point
           // - otherwise, no intersection
           if (intersection_test_segment_point(s, v, abs_tol, rel_tol)) {
             return r_t{v};
           } else {
             return r_t{monostate{}};
           }
         }
       else if
         constexpr(Segment<T>{}) {
           // If the ray intersect the aabb on a segment:
           // - compute the intersection between this and the original segment
           // (that's the result, might be the same segment, a part of the
           // segment, a point, or nothing)
           return r_t{intersection_segment_segment(s, v, abs_tol, rel_tol)};
         }
       else {
         // otherwise the ray does not intersect the AABB, so there is no
         // intersection.
         static_assert(Same<T, monostate>{});
         return r_t{monostate{}};
       }
     },
     ray_aabb_int);
  }
};

}  // namespace intersection_segment_aabb_detail

namespace {
static constexpr auto const& intersection_segment_aabb
 = static_const<with_default_tolerance<
  intersection_segment_aabb_detail::intersection_segment_aabb_fn>>::value;
}

namespace intersection_test_segment_aabb_detail {

struct intersection_test_segment_aabb_fn {
  /// Does the segment \p s intersect/contain the aabb \p p?
  template <typename T, typename U>
  constexpr bool operator()(T const& s, U const& p, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Segment<T>{});
    static_assert(AABB<U>{});
    static_assert(ad_v<T> == ad_v<U>);
    // TODO: speed this up, by avoiding computing the real intersection but
    // performing only the required intersection tests
    return visit(
     [](auto&& v) {
       using V = uncvref_t<decltype(v)>;
       if
         constexpr(Same<V, monostate>{}) { return false; }
       else {
         return true;
       }
     },
     intersection_segment_aabb(s, p, abs_tol, rel_tol));
  }
};

}  // namespace intersection_test_segment_aabb_detail

namespace {
static constexpr auto const& intersection_test_segment_aabb
 = static_const<with_default_tolerance<
  intersection_test_segment_aabb_detail::intersection_test_segment_aabb_fn>>::
  value;
}

}  // namespace hm3::geometry
