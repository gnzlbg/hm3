#pragma once
/// \file
///
/// Parameter of the intersection of a point with a segment.
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/algorithm/direction.hpp>
#include <hm3/geometry/algorithm/distance/minimum_segment_point.hpp>
#include <hm3/utility/optional.hpp>
#include <hm3/ext/variant.hpp>

namespace hm3::geometry {

namespace intersection_parameter_segment_point_detail {

struct on_segment_line_t {};
struct on_segment_t {};

static constexpr on_segment_line_t on_segment_line{};
static constexpr on_segment_t on_segment{};

struct intersection_parameter_segment_point_fn {
  /// Value of the segment parameter if the point \p lies in the line of the
  /// segment \p s such that for `p == s.x(0) => t =0` and `p == s.x(1) => t =
  /// 1`.
  ///
  /// If Q == on_segment_line_t: Returns nothing if the point does not line on
  /// the line spanned by the segment.
  ///
  /// If Q == on_segment_t: Returns nothing if the point does not line on the
  /// segment.
  template <typename S, typename P, typename Q = on_segment_t>
  constexpr optional<num_t> operator()(S const& s, P const& p, Q, num_t abs_tol,
                                       num_t rel_tol) const noexcept {
    static_assert(Segment<S>{});
    static_assert(Point<P>{});
    static_assert(ad_v<S> == ad_v<P>);
    constexpr auto ad = ambient_dimension(P{});
    const num_t dist  = [&]() {
      if
        constexpr(Same<Q, on_segment_line_t>{}) {
          return minimum_distance_line_point(s.line(), p);
        }
      else if
        constexpr(Same<Q, on_segment_t>{}) {
          return minimum_distance_segment_point(s, p, abs_tol, rel_tol);
          ;
        }
      else {
        static_assert(always_false<Q>{}, "unknown type-paramenter Q");
      }
    }();

    if (approx_number(dist, 0., abs_tol, rel_tol)) {
      const auto dir = direction(s);
      for (dim_t d = 0; d < ad; ++d) {
        if (!approx_number(dir(d), 0., abs_tol, rel_tol)) {
          return intersection_parameter_line_point_detail::t(s, p, d, abs_tol,
                                                             rel_tol);
        }
      }
      HM3_ASSERT(false, "segment: {}, direction vector has norm zero", s);
    }
    return {};
  }
};

}  // namespace intersection_parameter_segment_point_detail

using intersection_parameter_segment_point_detail::on_segment_line;
using intersection_parameter_segment_point_detail::on_segment;

namespace {
static constexpr auto const& intersection_parameter_segment_point
 = static_const<
  with_default_tolerance<intersection_parameter_segment_point_detail::
                          intersection_parameter_segment_point_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
