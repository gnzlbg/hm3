#pragma once
/// \file
///
/// Intersection of a range of segments with a polygon.
#include <hm3/geometry/algorithm/intersection/polygon_segment.hpp>

namespace hm3::geometry::polygon_primitive {
// TODO: this still works because ADL is able to find this for a particular
// polygon type, I don't think it can be made to work for intersection(Range<A>,
// Range<B>), so maybe it would be better to find another solution long term
// (e.g. instead of relying in adl, use tag dispatching).

/// Intersection of the segment \p s with the polygon \p p.
template <typename SegmentRng, typename P,
          typename S = range_value_t<SegmentRng>,
          dim_t Nd   = uncvref_t<S>::dimension(),
          CONCEPT_REQUIRES_(Range<SegmentRng>{} and Same<S, segment<Nd>>{}
                            and Polygon<P, Nd>{})>
variant<monostate, small_vector<segment<Nd>, 4>> intersection(
 SegmentRng&& segment_rng, P&& p) noexcept {
  HM3_ASSERT(
   convex(p),
   "polygon {} is not convex! (see comment in polygon_segment intersection)");
  using p_t = point<Nd>;
  using s_t = segment<Nd>;
  small_vector<s_t, 4> segments;

  for (auto&& s : segment_rng) {
    auto r = intersection(s, p);
    visit(
     [&](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if
         constexpr(Same<T, p_t>{}) {
           // do nothing
         }
       else if
         constexpr(Same<T, s_t>{}) { segments.push_back(v); }
       else if
         constexpr(Same<T, monostate>{}) {
           // do nothing
         }
       else {
         static_assert(fail<T>{}, "non-exhaustive variant");
       }
     },
     r);
  }
  if (size(segments) > 0) { return segments; }
  return monostate{};
}

}  // namespace hm3::geometry::polygon_primitive
