#pragma once
/// \file
///
/// Are two segments approximately equal
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/concept/segment.hpp>

namespace hm3::geometry {

namespace approx_segment_detail {

struct approx_segment_fn {
  template <typename T, typename NT>
  constexpr bool operator()(T const& a, T const& b, NT abs_tol,
                            NT rel_tol) const noexcept {
    static_assert(Segment<T>{});
    return approx_point(a.x(0), b.x(0), abs_tol, rel_tol)
           and approx_point(a.x(1), b.x(1), abs_tol, rel_tol);
  }
};

}  // namespace approx_segment_detail

namespace {
constexpr auto const& approx_segment = static_const<
 with_default_tolerance<approx_segment_detail::approx_segment_fn>>::value;

}  // namespace

}  // namespace hm3::geometry
