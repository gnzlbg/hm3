#pragma once
/// \file
///
/// Concatenate two non-overlapping polylines
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/algorithm/vertex.hpp>
#include <hm3/geometry/concept/polyline.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry {

namespace concatenate_polyline_polyline_detail {

struct concatenate_polyline_polyline_fn {
  template <typename P>

  constexpr bool non_overlapping(P&& /*l0*/, P&& /*l1*/) const {
    // TODO:
    //
    // 1. Does any segment of l0 intersect any segment of l1? If no, they do not
    // overlap.
    //
    // 2. If yes and the intersection is on a point, if the intersection is on a
    // point, check if this point is one of the end points of the polylines. (if
    // so, they do not overlap, otherwise, they overlap).
    return true;
  }

  /// Set union between two polylines.
  template <typename P0, typename P1, typename UP0 = uncvref_t<P0>,
            typename UP1 = uncvref_t<P1>,
            CONCEPT_REQUIRES_(Same<UP0, UP1>{} and MutablePolyline<UP0>{})>
  constexpr optional<UP0> operator()(P0&& l0, P1&& l1, num_t abs_tol,
                                     num_t rel_tol) const noexcept {
    HM3_ASSERT(vertex_size(l0) > 1, "");
    HM3_ASSERT(vertex_size(l1) > 1, "");
    HM3_ASSERT(non_overlapping(l0, l1), "");

    auto first0 = vertex(l0, 0);
    auto last0  = vertex(l0, vertex_size(l0) - 1);

    auto first1 = vertex(l1, 0);
    auto last1  = vertex(l1, vertex_size(l1) - 1);

    auto from_edges = [](auto&& edges0, auto&& edges1) {
      UP0 tmp;
      tmp.reserve(ranges::size(edges0) + ranges::size(edges1));
      for (auto&& e : edges0) { tmp.push_back_edge(e); }
      for (auto&& e : edges1) { tmp.push_back_edge(e); }
      return tmp;
    };

    if (approx(last0, first1, abs_tol,
               rel_tol)) {  // insert l1 at the end of l0
      return from_edges(edges(l0), edges(l1));
    }
    if (approx(last1, first0, abs_tol,
               rel_tol)) {  // insert l0 at the end of l1
      return from_edges(edges(l1), edges(l0));
    }
    return {};
  }
};

}  // namespace concatenate_polyline_polyline_detail

namespace {
constexpr auto const& concatenate_polyline_polyline
 = static_const<with_default_tolerance<
  concatenate_polyline_polyline_detail::concatenate_polyline_polyline_fn>>::
  value;
}

}  // namespace hm3::geometry
