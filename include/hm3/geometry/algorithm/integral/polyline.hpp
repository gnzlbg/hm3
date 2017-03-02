#pragma once
/// \file
///
/// Integral of a Polyline.
#include <hm3/geometry/algorithm/distance/minimum_point_point.hpp>
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/algorithm/integral/segment.hpp>
#include <hm3/geometry/concept/polyline.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace integral_polyline_detail {

struct integral_polyline_fn {
  /// Path integral of the polyline \p s.
  template <typename T>
  constexpr associated::num_type_t<T> operator()(
   T&& s, trait::path_integral<T>) const {
    static_assert(Polyline<uncvref_t<T>>{});
    num_t l = 0.;
    for (auto&& e : edges(s)) {
      l += minimum_distance_point_point(e.x(0), e.x(1));
    }
    return l;
  }
};

}  // namespace integral_polyline_detail

namespace {
static constexpr auto const& integral_polyline
 = static_const<integral_polyline_detail::integral_polyline_fn>::value;
}  // namespace

}  // namespace hm3::geometry
