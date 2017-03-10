#pragma once
/// \file
///
/// Centroid of a polyline
#include <hm3/geometry/algorithm/centroid/segment.hpp>
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/algorithm/integral/segment.hpp>
#include <hm3/geometry/concept/polyline.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace centroid_polyline_detail {

struct centroid_polyline_fn {
  /// Centroid coordinates of the polyline \p s.
  template <typename P>
  constexpr auto operator()(P&& p) const noexcept {
    static_assert(Polyline<uncvref_t<P>>{});
    using p_t = associated::point_t<P>;
    using e_t = associated::edge_t<P>;

    HM3_ASSERT(edge_size(p) > 0,
               "cannot compute the centroid of an empty polyline");

    auto x_c         = p_t::constant(0.);
    num_t length_sum = 0.;

    for (auto&& s : edges(p)) {
      auto segment_l  = integral_segment(s, trait::path_integral<e_t>{});
      auto segment_xc = centroid_segment(s);
      x_c() += segment_l * segment_xc();
      length_sum += segment_l;
    }

    HM3_ASSERT(length_sum > 0., "sum of the polyline lengths is {} !> 0.",
               length_sum);
    x_c() /= length_sum;
    return x_c;
  }
};

}  // namespace centroid_polyline_detail

namespace {
constexpr auto const& centroid_polyline
 = static_const<centroid_polyline_detail::centroid_polyline_fn>::value;
}

}  // namespace hm3::geometry
