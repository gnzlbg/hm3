#pragma once
/// \file
///
/// Intersection of a range of segments with a polygon.
#include <hm3/geometry/algorithm/intersection/polygon_segment.hpp>
#include <hm3/geometry/algorithm/is_convex.hpp>

namespace hm3::geometry {

namespace intersection_polygon_segments_detail {

struct intersection_polygon_segments_fn {
  /// Intersection of the polygon \p p with a range of segments \p rng
  template <typename P, typename SegmentRng>
  constexpr auto operator()(P&& p, SegmentRng&& segment_rng) const noexcept {
    using pl_t = uncvref_t<P>;
    static_assert(Polygon<pl_t>{});
    using s_t = uncvref_t<ranges::range_value_t<SegmentRng>>;
    static_assert(Segment<s_t>{});
    static_assert(ambient_dimension_v<pl_t> == ambient_dimension_v<s_t>);
    static_assert(ambient_dimension_v<pl_t> == ambient_dimension_v<s_t>);
    static_assert(not std::is_rvalue_reference<P>{});
    static_assert(not std::is_rvalue_reference<SegmentRng>{});
    HM3_ASSERT(is_convex(p), "polygon is not convex!\n\npolygon: {})", p);

    return segment_rng | view::for_each([&](auto&& s) {
             auto i      = intersection_polygon_segment(p, s);
             auto* i_ptr = get_if<s_t>(&i);
             return ranges::yield_if(i_ptr != nullptr, *i_ptr);
           });
  }
};

}  // namespace intersection_polygon_segments_detail

namespace {
constexpr auto const& intersection_polygon_segments = static_const<
 intersection_polygon_segments_detail::intersection_polygon_segments_fn>::value;
}

}  // namespace hm3::geometry
