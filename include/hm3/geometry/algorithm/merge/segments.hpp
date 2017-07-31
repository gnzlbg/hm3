#pragma once
/// \file
///
/// Merges two segments into one.
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/algorithm/collinear.hpp>
#include <hm3/geometry/concept/segment.hpp>
#include <hm3/geometry/primitive/data.hpp>
#include <hm3/geometry/tolerance.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry {

namespace merge_segments_detail {

struct merge_segments_fn {
  /// Merge the segments \p pl by merging colinear adjacent edges.
  template <typename S>
  constexpr auto operator()(S const& s0, S const& s1, num_t abs_tol,
                            num_t rel_tol) const noexcept -> optional<S> {
    static_assert(Segment<S>{});
    if (not approx_point(s0.x(1), s1.x(0), abs_tol, rel_tol)) { return {}; }
    if (not collinear(s0, s1, abs_tol, rel_tol)) { return {}; }
    if constexpr (HasData<S>{} or HasData<associated::point_t<S>>{}) {
      if constexpr (HasData<S>{}) {
        // cannot merge segments with data that is not the same
        auto&& d0 = data(s0);
        auto&& d1 = data(s1);
        if (d0 != d1) { return {}; }
      }
      if constexpr (HasData<associated::point_t<S>>{}) {
        // merging removes s0.x(1) and s1.x(0):
        // - cannot remove s0.x(1) if its data is not equal to s1.x(1)
        // - cannot remove s1.x(0) if its data is not equal to s0.x(0)
        auto&& ds0p0 = data(s0.x(0));
        auto&& ds0p1 = data(s0.x(1));
        auto&& ds1p0 = data(s1.x(0));
        auto&& ds1p1 = data(s1.x(1));
        if (ds0p0 != ds1p0) { return {}; }
        if (ds0p1 != ds1p1) { return {}; }
      }
      S result(s0);
      result.x(1) = s1.x(1);
      return result;
    } else {
      return S(s0.x(0), s1.x(1));
    }
  }
};

}  // namespace merge_segments_detail

namespace {
constexpr auto const& merge_segments = static_const<
 with_default_tolerance<merge_segments_detail::merge_segments_fn>>::value;
}

}  // namespace hm3::geometry
