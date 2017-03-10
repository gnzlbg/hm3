#pragma once
/// \file
///
/// Centroid of a segment
#include <hm3/geometry/concept/segment.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace centroid_segment_detail {

struct centroid_segment_fn {
  /// Centroid coordinates of the segment \p s.
  template <typename P>
  constexpr auto operator()(P&& s) const noexcept {
    static_assert(Segment<uncvref_t<P>>{});
    using PT = associated::point_t<P>;
    return PT(s.x(0)() + 0.5 * (s.x(1)() - s.x(0)()));
  }
};

}  // namespace centroid_segment_detail

namespace {
constexpr auto const& centroid_segment
 = static_const<centroid_segment_detail::centroid_segment_fn>::value;
}

}  // namespace hm3::geometry
