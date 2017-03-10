#pragma once
/// \file
///
/// Split line segment at a point.
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/algorithm/intersection/segment_point.hpp>
#include <hm3/utility/fixed_capacity_vector.hpp>

namespace hm3::geometry {

namespace split_segment_point_detail {

struct split_segment_point_fn {
  /// Splits the segment \p s at the point \p p.
  template <typename S, typename P>
  constexpr fixed_capacity_vector<S, 2> operator()(S const& s, P const& p) const
   noexcept {
    static_assert(Segment<S>{});
    static_assert(Point<P>{});
    static_assert(ad_v<S> == ad_v<P>);
    static_assert(Same<associated::point_t<S>, P>{});

    auto ir = intersection_segment_point(s, p);

    fixed_capacity_vector<S, 2> result;

    visit(
     [&](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if
         constexpr(Same<T, P>{}) {
           if (approx_point(s.x(0), p) or approx_point(s.x(1), p)) {
             result.push_back(s);
             return;
           }
           result.push_back(S(s.x(0), p));
           result.push_back(S(p, s.x(1)));
         }
       else if
         constexpr(Same<T, monostate>{}) { return; }
       else {
         HM3_STATIC_ASSERT_EXHAUSTIVE_VISITOR(T);
       }
     },
     ir);
    return result;
  }
};

}  // namespace split_segment_point_detail

namespace {
constexpr auto const& split_segment_point
 = static_const<split_segment_point_detail::split_segment_point_fn>::value;
}

}  // namespace hm3::geometry
