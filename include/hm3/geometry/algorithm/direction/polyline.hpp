#pragma once
/// \file
///
/// Invert the direction of a polyline:
///
/// The direction of a polyline as a vector does not make sense (so we cannot
/// compute it), but we can still invert it by reversing the order of segments.
#include <hm3/geometry/algorithm/direction/segment.hpp>
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/concept/polyline.hpp>

namespace hm3::geometry {

namespace direction_polyline_detail {

struct direction_polyline_fn {
  /// Invert direction of polyline \p s.
  template <typename T>
  static constexpr T invert_impl(T const& s) noexcept {
    using UT = uncvref_t<T>;
    static_assert(MutablePolyline<UT>{});
    UT tmp;
    tmp.reserve(edge_size(s));
    for (auto&& e : edges(s) | view::reverse) {
      tmp.push_back_edge(direction_segment.invert(e));
    }
    return tmp;
  }

  // note: no operator() since polylines have no intrinsic concept of direction.

  template <typename T>
  static constexpr auto invert(T&& t)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(invert_impl(std::forward<T>(t)));
};

}  // namespace direction_polyline_detail

namespace {
constexpr auto const& direction_polyline
 = static_const<direction_polyline_detail::direction_polyline_fn>::value;
}  // namespace

}  // namespace hm3::geometry
