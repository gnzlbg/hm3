#pragma once
/// \file
///
/// Are two lines / segments collinear?
#include <hm3/geometry/algorithm/approx/line.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/concept/segment.hpp>

namespace hm3::geometry {

namespace collinear_detail {

struct collinear_fn {
  /// Two lines are collinear if they are coincidental.
  template <typename L, CONCEPT_REQUIRES_(Line<L>{})>
  constexpr bool operator()(L const& l0, L const& l1, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    return coincidental_lines(l0, l1, abs_tol, rel_tol);
  }

  /// Two segments are collinear if they span collinear lines.
  template <typename S, CONCEPT_REQUIRES_(Segment<S>{})>
  constexpr bool operator()(S const& s0, S const& s1, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    return (*this)(s0.line(), s1.line(), abs_tol, rel_tol);
  }
};

}  // namespace collinear_detail

namespace {
constexpr auto const& collinear
 = static_const<with_default_tolerance<collinear_detail::collinear_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
