#pragma once
/// \file
///
/// Are two lines approximately equal or coincidental
#include <hm3/geometry/algorithm/parallel/vector.hpp>
#include <hm3/geometry/concept/line.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/concept/vector.hpp>
#include <hm3/geometry/tolerance.hpp>

namespace hm3::geometry {

namespace approx_line_detail {

struct approx_line_fn {
  /// Are the lines (p1, v1) and (p2, v2) coincidental?
  template <typename P, typename V, typename NT>
  constexpr bool operator()(P const& p1, V const& v1,  //
                            P const& p2, V const& v2,  //
                            NT abs_tol, NT rel_tol) const noexcept {
    static_assert(Point<P>{});
    static_assert(Vector<V>{});
    if (!parallel_vector(v1, v2, abs_tol, rel_tol)) { return false; }
    auto w = V(p1() - p2());
    return parallel_vector(w, v2, abs_tol, rel_tol);
  }

  template <typename T, typename NT,
            CONCEPT_REQUIRES_(not AmbientDimension<T, 1>{})>
  constexpr bool operator()(T const& a, T const& b, NT abs_tol,
                            NT rel_tol) const noexcept {
    static_assert(Line<T>{});
    return approx_line_fn{}(a.origin(), a.direction(), b.origin(),
                            b.direction(), abs_tol, rel_tol);
  }

  /// 1D lines are always equal:
  template <typename T, typename NT,
            CONCEPT_REQUIRES_(AmbientDimension<T, 1>{})>
  constexpr bool operator()(T const&, T const&, NT, NT) const noexcept {
    static_assert(Line<T>{});
    return true;
  }
};

}  // namespace approx_line_detail

namespace {
constexpr auto const& approx_line = static_const<
 with_default_tolerance<approx_line_detail::approx_line_fn>>::value;
constexpr auto const& coincidental_lines = approx_line;
}  // namespace

}  // namespace hm3::geometry
