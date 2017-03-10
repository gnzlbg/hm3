#pragma once
/// \file
///
/// Concatenate a polyline and a segment.
#include <hm3/geometry/algorithm/concatenate/polyline_polyline.hpp>
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/algorithm/vertex.hpp>
#include <hm3/geometry/concept/polyline.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry {

namespace concatenate_segment_polyline_detail {

struct concatenate_segment_polyline_fn {
  /// Set union between two polylines.
  template <typename S, typename P, typename US = uncvref_t<S>,
            typename UP = uncvref_t<P>,
            CONCEPT_REQUIRES_(Segment<US>{} and MutablePolyline<UP>{}
                              and Same<associated::edge_t<UP>, US>{})>
  constexpr optional<UP> operator()(S&& s, P&& l, num_t abs_tol,
                                    num_t rel_tol) const noexcept {
    return concatenate_polyline_polyline(UP({s}), l, abs_tol, rel_tol);
  }
};

}  // namespace concatenate_segment_polyline_detail

namespace {
constexpr auto const& concatenate_segment_polyline
 = static_const<with_default_tolerance<
  concatenate_segment_polyline_detail::concatenate_segment_polyline_fn>>::value;
}

}  // namespace hm3::geometry
