#pragma once
/// \file
///
/// Intersection of line segments.
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/algorithm/line_intersection_parameter/line_line.hpp>
#include <hm3/geometry/algorithm/parallel.hpp>
#include <hm3/geometry/concept/segment.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry {

namespace intersection_test_segment_segment_detail {

/// Test the intersection of two monotonic 1D segments [a_0, a_1] and [b_0,
/// b_1].
///
/// \pre a_0 <= a_1 and b_0 <= b_1
inline bool test_monotonic_1d(num_t a_0, num_t a_1, num_t b_0, num_t b_1,
                              num_t abs_tol, num_t rel_tol) noexcept {
  HM3_ASSERT(approx.leq(a_0, a_1, abs_tol, rel_tol), "");
  HM3_ASSERT(approx.leq(b_0, b_1, abs_tol, rel_tol), "");
  return approx.geq(a_1, b_0, abs_tol, rel_tol)
         and approx.geq(b_1, a_0, abs_tol, rel_tol);
}

/// Test the intersection of two 1D segments [a_0, a_1] and [b_0, b_1].
inline bool test_1d(num_t a_0, num_t a_1, num_t b_0, num_t b_1, num_t abs_tol,
                    num_t rel_tol) noexcept {
  num_t a0 = std::min(a_0, a_1);
  num_t a1 = std::max(a_0, a_1);

  num_t b0 = std::min(b_0, b_1);
  num_t b1 = std::max(b_0, b_1);

  return test_monotonic_1d(a0, a1, b0, b1, abs_tol, rel_tol);
}

/// Intersection of two 1D segments [a_0, a_1] and [b_0, b_1].
inline optional<pair<num_t, num_t>> intersection_1d(num_t a_0, num_t a_1,
                                                    num_t b_0, num_t b_1,
                                                    num_t abs_tol,
                                                    num_t rel_tol) noexcept {
  num_t a0 = std::min(a_0, a_1);
  num_t a1 = std::max(a_0, a_1);

  num_t b0 = std::min(b_0, b_1);
  num_t b1 = std::max(b_0, b_1);

  if (!test_monotonic_1d(a0, a1, b0, b1, abs_tol, rel_tol)) { return {}; }

  num_t first  = a0 < b0 ? b0 : a0;
  num_t second = a1 > b1 ? b1 : a1;

  return make_pair(first, second);
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
template <typename T, typename U>
inline bool test_nd(T const& a, U const& b, num_t abs_tol,
                    num_t rel_tol) noexcept {
  static_assert(ad_v<T>> 1);
  static_assert(ad_v<T> == ad_v<U>);
  static_assert(Segment<T>{});
  static_assert(Segment<U>{});

  using v_t = associated::vector_t<T>;

  auto a0        = a.x(0);
  auto b0        = b.x(0);
  auto a_d       = v_t(a.x(1)() - a.x(0)());
  auto b_d       = v_t(b.x(1)() - b.x(0)());
  auto b0_min_a0 = b0() - a0();

  if (parallel(a_d, b_d, abs_tol, rel_tol)) {
    if (!coincidental_lines(a0, a_d, b0, b_d, abs_tol, rel_tol)) {
      return false;
    }

    auto ad_dot_ad = a_d().dot(a_d());
    auto bd_dot_ad = b_d().dot(a_d());
    auto t0        = b0_min_a0.dot(a_d()) / ad_dot_ad;
    auto t1        = t0 + bd_dot_ad / ad_dot_ad;

    return test_1d(t0, t1, 0., 1., abs_tol, rel_tol);
  }

  auto is
   = intersection_parameter_line_line(a0, a_d, b0, b_d, abs_tol, rel_tol);
  if (!is) { return false; }
  auto t = first(is.value());
  auto s = second(is.value());
  if (approx.leq(0., t, abs_tol, rel_tol)
      and approx.leq(t, 1., abs_tol, rel_tol)
      and approx.leq(0., s, abs_tol, rel_tol)
      and approx.leq(s, 1., abs_tol, rel_tol)) {
    return true;  // both segments intersect
  }
  return false;  // Case 3: not parallel and not intersect
}

struct intersection_test_segment_segment_fn {
  /// Do two line segments in 1D intersect?
  template <typename T, typename U>
  static constexpr bool impl(T const& a, U const& b, trait::segment<1>,
                             num_t abs_tol, num_t rel_tol) noexcept {
    return test_1d(a.x(0)(0), a.x(1)(0), b.x(0)(0), b.x(1)(0), abs_tol,
                   rel_tol);
  }

  /// Do two line segments in 2D intersect?
  template <typename T, typename U>
  static constexpr bool impl(T const& a, U const& b, trait::segment<2>,
                             num_t abs_tol, num_t rel_tol) noexcept {
    return test_nd(a, b, abs_tol, rel_tol);
  }

  /// Do two line segments in 3D intersect?
  template <typename T, typename U>
  static constexpr bool impl(T const& a, U const& b, trait::segment<3>,
                             num_t abs_tol, num_t rel_tol) noexcept {
    return test_nd(a, b, abs_tol, rel_tol);
  }

  /// Does the one dimensional segments \p s and \p u intersect?
  static bool one_dimensional(num_t s0, num_t s1, num_t u0, num_t u1,
                              num_t abs_tol, num_t rel_tol) noexcept {
    return test_1d(s0, s1, u0, u1, abs_tol, rel_tol);
  }

  template <typename T, typename U>
  constexpr bool operator()(T const& a, U const& b, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Segment<T>{});
    static_assert(Segment<U>{});
    static_assert(ad_v<T> == ad_v<U>);

    return impl(a, b, associated::v_<T>, abs_tol, rel_tol);
  }
};

}  // namespace intersection_test_segment_segment_detail

namespace {
static constexpr auto const& intersection_test_segment_segment = static_const<
 with_default_tolerance<intersection_test_segment_segment_detail::
                         intersection_test_segment_segment_fn>>::value;
}

namespace intersection_segment_segment_detail {

/// Intersection between two 2D/3D segments
template <typename T, typename U, typename ET = uncvref_t<T>,
          typename PT = associated::point_t<T>>
constexpr auto intersection_nd(T const& a, U const& b, num_t abs_tol,
                               num_t rel_tol) noexcept
 -> variant<monostate, ET, PT> {
  using v_t = associated::vector_t<T>;
  auto a0   = a.x(0);
  auto b0   = b.x(0);
  auto a_d  = v_t(a.x(1)() - a.x(0)());
  auto b_d  = v_t(b.x(1)() - b.x(0)());

  // TODO: pass tolerance here?
  if (parallel(a_d, b_d, abs_tol, rel_tol)) {
    // TODO: pass tolerance here?
    if (not coincidental_lines(a0, a_d, b0, b_d, abs_tol, rel_tol)) {
      return monostate{};
    }
    auto ad_dot_ad = a_d().dot(a_d());
    auto bd_dot_ad = b_d().dot(a_d());
    auto t0        = (b0() - a0()).dot(a_d()) / ad_dot_ad;
    auto t1        = t0 + bd_dot_ad / ad_dot_ad;

    auto ps = intersection_test_segment_segment_detail::intersection_1d(
     t0, t1, 0., 1., abs_tol, rel_tol);
    if (!ps) { return monostate{}; }

    num_t t = first(ps.value());
    num_t u = second(ps.value());

    auto p0 = PT(a0() + t * a_d());
    if (approx_number(t, u, abs_tol, rel_tol)) { return p0; }
    auto p1 = PT(a0() + u * a_d());
    return ET(p0, p1);
  }

  auto is
   = intersection_parameter_line_line(a0, a_d, b0, b_d, abs_tol, rel_tol);
  if (!is) { return monostate{}; }
  auto t = first(is.value());
  auto s = second(is.value());
  if (approx.leq(0., t, abs_tol, rel_tol)
      and approx.leq(t, 1., abs_tol, rel_tol)
      and approx.leq(0., s, abs_tol, rel_tol)
      and approx.leq(s, 1., abs_tol, rel_tol)) {
    return PT{a0() + t * a_d()};  // both segments intersect
  }
  return monostate{};  // Case 3: not parallel and not intersect
}

struct intersection_segment_segment_fn {
  /// Intersection point of the one dimensional segments \p s and \p u
  /// intersect?
  static auto one_dimensional(num_t s0, num_t s1, num_t u0, num_t u1,
                              num_t abs_tol, num_t rel_tol) noexcept {
    return intersection_test_segment_segment_detail::intersection_1d(
     s0, s1, u0, u1, abs_tol, rel_tol);
  }

  /// Intersection between two 1D segments
  template <typename T, typename U, typename ET = uncvref_t<T>,
            typename PT = associated::point_t<T>>
  static constexpr auto impl(T const& a, U const& b, trait::segment<1>,
                             num_t abs_tol, num_t rel_tol) noexcept
   -> variant<monostate, ET, PT> {
    auto result = one_dimensional(a.x(0)(0), a.x(1)(0), b.x(0)(0), b.x(1)(0),
                                  abs_tol, rel_tol);

    if (!result) { return monostate{}; }

    auto r  = result.value();
    auto x0 = PT{first(r)};
    auto x1 = PT{second(r)};
    if (approx_point(x0, x1, abs_tol, rel_tol)) { return x0; }
    return ET(x0, x1);
  }

  /// Intersection between two 1D segments
  template <typename T, typename U, typename ET = uncvref_t<T>,
            typename PT = associated::point_t<T>>
  static constexpr auto impl(T const& a, U const& b, trait::segment<2>,
                             num_t abs_tol, num_t rel_tol) noexcept {
    return intersection_nd(a, b, abs_tol, rel_tol);
  }

  /// Intersection between two 1D segments
  template <typename T, typename U, typename ET = uncvref_t<T>,
            typename PT = associated::point_t<T>>
  static constexpr auto impl(T const& a, U const& b, trait::segment<3>,
                             num_t abs_tol, num_t rel_tol) noexcept {
    return intersection_nd(a, b, abs_tol, rel_tol);
  }

  template <typename T, typename U>
  constexpr auto operator()(T const& a, U const& b, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Segment<T>{});
    static_assert(Segment<U>{});
    static_assert(ad_v<T> == ad_v<U>);
    return impl(a, b, associated::v_<T>, abs_tol, rel_tol);
  }
};

}  // namespace intersection_segment_segment_detail

namespace {
static constexpr auto const& intersection_segment_segment
 = static_const<with_default_tolerance<
  intersection_segment_segment_detail::intersection_segment_segment_fn>>::value;
}

}  // namespace hm3::geometry::segment_primitive
