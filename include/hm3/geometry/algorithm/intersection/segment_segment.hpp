#pragma once
/// \file
///
/// Intersection of line segments.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/primitive/line/coincidental.hpp>
#include <hm3/geometry/primitive/line/parallel.hpp>
#include <hm3/geometry/primitive/line/parameter.hpp>
#include <hm3/geometry/primitive/segment/direction.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::segment_primitive {

namespace segment_intersection_detail {

/// Test the intersection of two monotonic 1D segments [a_0, a_1] and [b_0,
/// b_1].
///
/// \pre a_0 <= a_1 and b_0 <= b_1
inline bool test_monotonic_1d(num_t a_0, num_t a_1, num_t b_0,
                              num_t b_1) noexcept {
  HM3_ASSERT(a_0 <= a_1, "");
  HM3_ASSERT(b_0 <= b_1, "");
  return a_1 >= b_0 and b_1 >= a_0;
}

/// Test the intersection of two 1D segments [a_0, a_1] and [b_0, b_1].
inline bool test_1d(num_t a_0, num_t a_1, num_t b_0, num_t b_1) noexcept {
  num_t a0 = std::min(a_0, a_1);
  num_t a1 = std::max(a_0, a_1);

  num_t b0 = std::min(b_0, b_1);
  num_t b1 = std::max(b_0, b_1);

  return test_monotonic_1d(a0, a1, b0, b1);
}

/// Intersection of two 1D segments [a_0, a_1] and [b_0, b_1].
inline optional<std::pair<num_t, num_t>> intersection_1d(num_t a_0, num_t a_1,
                                                         num_t b_0,
                                                         num_t b_1) noexcept {
  num_t a0 = std::min(a_0, a_1);
  num_t a1 = std::max(a_0, a_1);

  num_t b0 = std::min(b_0, b_1);
  num_t b1 = std::max(b_0, b_1);

  if (!test_monotonic_1d(a0, a1, b0, b1)) { return {}; }

  num_t first  = a0 < b0 ? b0 : a0;
  num_t second = a1 > b1 ? b1 : a1;

  return std::make_pair(first, second);
}

}  // namespace segment_intersection_detail

/// Do two line segments in 1D intersect?
inline bool intersection_test(segment<1> const& a,
                              segment<1> const& b) noexcept {
  return segment_intersection_detail::test_1d(a.x(0)(0), a.x(1)(0), b.x(0)(0),
                                              b.x(1)(0));
}

/// Do two line segments \p a and \p b intersect?
///
/// Basically same as in line intersection but:
///  - when the segment lines are coincidental, it is checked whether the
///    segments overlap.
///  - when the segment lines intersect on a point, it is checked whether the
///    point lies within both segments.
///
/// Implemented similar to how it is explained
/// [here](http://stackoverflow.com/a/565282) but probably not as efficient.
template <dim_t Nd, CONCEPT_REQUIRES_(Nd > 1)>
inline bool intersection_test(segment<Nd> const& a,
                              segment<Nd> const& b) noexcept {
  auto a0        = a.x(0);
  auto b0        = b.x(0);
  auto a_d       = vec<Nd>(a.x(1)() - a.x(0)());
  auto b_d       = vec<Nd>(b.x(1)() - b.x(0)());
  auto b0_min_a0 = b0() - a0();

  if (parallel(a_d, b_d)) {
    if (!line_primitive::coincidental(a0, a_d, b0, b_d)) { return false; }

    auto ad_dot_ad = a_d().dot(a_d());
    auto bd_dot_ad = b_d().dot(a_d());
    auto t0        = b0_min_a0.dot(a_d()) / ad_dot_ad;
    auto t1        = t0 + bd_dot_ad / ad_dot_ad;

    return segment_intersection_detail::test_1d(t0, t1, 0., 1.);
  }

  auto is = line_primitive::parameter(a0, a_d, b0, b_d);
  if (!is) { return false; }
  auto t = is.value().first;
  auto s = is.value().second;
  if (0. <= t and t <= 1. and 0. <= s and s <= 1.) {
    return true;  // both segments intersect
  }
  return false;  // Case 3: not parallel and not intersect
}

/// Intersection between two 1D segments
variant<monostate, segment<1>, point<1>> intersection(
 segment<1> const& a, segment<1> const& b) noexcept {
  using p_t = point<1>;
  using s_t = segment<1>;

  auto result = segment_intersection_detail::intersection_1d(
   a.x(0)(0), a.x(1)(0), b.x(0)(0), b.x(1)(0));
  if (!result) { return monostate{}; }

  auto x0 = p_t(result.value().first);
  auto x1 = p_t(result.value().second);

  if (x0 == x1) { return x0; }
  return s_t::through(x0, x1);
}

/// Intersection between two 2D/3D segments
template <dim_t Nd>
variant<monostate, segment<Nd>, point<Nd>> intersection(
 segment<Nd> const& a, segment<Nd> const& b) noexcept {
  using p_t      = point<Nd>;
  using s_t      = segment<Nd>;
  using v_t      = vec<Nd>;
  auto a0        = a.x(0);
  auto b0        = b.x(0);
  auto a_d       = v_t(a.x(1)() - a.x(0)());
  auto b_d       = v_t(b.x(1)() - b.x(0)());
  auto b0_min_a0 = b0() - a0();

  if (parallel(a_d, b_d)) {
    if (!line_primitive::coincidental(a0, a_d, b0, b_d)) { return monostate{}; }
    auto ad_dot_ad = a_d().dot(a_d());
    auto bd_dot_ad = b_d().dot(a_d());
    auto t0        = b0_min_a0.dot(a_d()) / ad_dot_ad;
    auto t1        = t0 + bd_dot_ad / ad_dot_ad;

    auto ps = segment_intersection_detail::intersection_1d(t0, t1, 0., 1.);
    if (!ps) { return monostate{}; }

    auto p0 = p_t(a0() + ps.value().first * a_d());
    if (math::approx(ps.value().first, ps.value().second)) { return p0; }
    auto p1 = p_t(a0() + ps.value().second * a_d());
    return s_t::through(p0, p1);
  }

  auto is = line_primitive::parameter(a0, a_d, b0, b_d);
  if (!is) { return monostate{}; }
  auto t = is.value().first;
  auto s = is.value().second;
  if (0. <= t and t <= 1. and 0. <= s and s <= 1.) {
    return p_t{a0() + t * a_d()};  // both segments intersect
  }
  return monostate{};  // Case 3: not parallel and not intersect
}

}  // namespace hm3::geometry::segment_primitive
