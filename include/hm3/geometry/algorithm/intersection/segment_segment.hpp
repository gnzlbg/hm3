#pragma once
/// \file
///
/// Intersection of line segments.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::segment_primitive {

/// Do two line segments in 1D intersect?
inline bool intersection_test(segment<1> const& a,
                              segment<1> const& b) noexcept {
  HM3_ASSERT(a.x(0) < a.x(1), "");
  HM3_ASSERT(b.x(0) < b.x(1), "");
  return !(a.x(1) < b.x(0) or b.x(1) < a.x(0));
}

/// Do two line segments in 2D intersect?
///
/// Implemented as explained [here](http://stackoverflow.com/a/565282):
///
/// Capital letters denote vecotrs, otherwise scalars are meant. The cross
/// product `x` is defined as the magnitude of the 3D cross product:
///
/// V x W = V_x * W_y - V_y * W_x
///
/// The line equations of the segments `a` and `b` are written as:
///
/// A(t) = A_0 + t (A_1 - A_0) = A_0 + t A_D
/// B(u) = B_0 + u (B_1 - B_0) = B_0 + u B_D
///
/// There are 4 cases:
///
/// 1. If A_D x B_d ~= 0 both segments are colinear:
///
///   1.1: If (B_0 - A_0) x A_D !~= 0: segments are parallel but not intersect.
///   1.2: If (B_0 - A_0) x A_D ~= 0: segments are parallel and intersect if:
///        - Given:
///            t0 = (B_0 - A_0) · A_D / (A_D · A_D)
///            t1 = t0 + (B_D · A_D) / (A_D · A_D)
///        - the interval [t0,t1] intersects [0, 1] if B_D · A_D >= 0.,
///        - the interval [t1,t0] intersects [0, 1] if B_D · A_D < 0.
///
/// 2.If A_D x B_d !~= 0 and given:
///    - t = (B_0 - A_0) x B_d / (A_d x B_D)
///    - u = (B_0 - A_0) x A_d / (A_d x B_D)
///   the segments intersect at the point A_0 + t A_D == B_0 + u B_d if
///   0 <= t <= 1 and 0 <= u <= 1.
///
/// 3. Otherwise the segments don't intersect.
///
inline bool intersection_test(segment<2> const& a,
                              segment<2> const& b) noexcept {
  auto a_0 = a.x(0)();        // p
  auto b_0 = b.x(0)();        // q
  auto a_d = a.x(1)() - a_0;  // r
  auto b_d = b.x(1)() - b_0;  // s

  // The cross-product here is the magnitude of the 3D cross product:
  auto cross
   = [](auto&& v, auto&& w) -> num_t { return v(0) * w(1) - v(1) * w(0); };

  auto ad_cross_bd = cross(a_d, b_d);
  auto b0_min_a0   = b_0 - a_0;

  // Case 1: colinear?
  if (math::approx(ad_cross_bd, 0.)) {
    auto b0_min_a0_cross_ad = cross(b0_min_a0, a_d);
    if (!math::approx(b0_min_a0_cross_ad, 0.)) {
      return false;  // Case 1.1: parallel and non-intersecting
    }

    // Case 1.2: parallel and maybe intersecting along a segment
    auto ad_dot_ad = a_d.dot(a_d);
    auto bd_dot_ad = b_d.dot(a_d);
    auto t0        = b0_min_a0.dot(a_d) / ad_dot_ad;
    auto t1        = t0 + bd_dot_ad / ad_dot_ad;

    auto intersects_zero_one = [](auto&& v0, auto&& v1) {
      HM3_ASSERT(v0 <= v1, "");
      if (v0 <= 0. and v1 >= 0.) { return true; }
      if (v0 >= 0. and v0 <= 1.) { return true; }
      return false;
    };

    if (bd_dot_ad >= 0.) { return intersects_zero_one(t0, t1); }
    return intersects_zero_one(t1, t0);
  }

  // Case 2: non-parallel, intersect at a point?
  auto b0_min_a0_cross_bd = cross(b0_min_a0, b_d);
  auto t                  = b0_min_a0_cross_bd / ad_cross_bd;
  auto b0_min_a0_cross_ad = cross(b0_min_a0, a_d);
  auto u                  = b0_min_a0_cross_ad / ad_cross_bd;

  if (0. <= t and t <= 1. and 0. <= u and u <= 1.) {
    return true;  // both lines intersect at a point
  }
  return false;  // Case 3: not parallel and not intersect
}

// Do two line segments in 3D intersect?
inline bool intersection_test(segment<3> const& a,
                              segment<3> const& b) noexcept {
  HM3_FATAL_ERROR("not implemented");
  return false;
}

}  // namespace hm3::geometry::segment_primitive
