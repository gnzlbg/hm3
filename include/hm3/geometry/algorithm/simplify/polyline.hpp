#pragma once
/// \file
///
/// Simplifies a polyline: merges colinear adjacent edges.
#include <hm3/geometry/algorithm/concatenate/segment_segment.hpp>
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/concept/polyline.hpp>
#include <hm3/geometry/fwd.hpp>

#include <hm3/io/ascii.hpp>

namespace hm3::geometry {

namespace simplify_polyline_detail {

struct simplify_polyline_fn {
  /// Simplify the polyline \p pl by merging colinear adjacent edges.
  template <typename P, typename UP = uncvref_t<P>,
            CONCEPT_REQUIRES_(MutablePolyline<UP>{})>
  constexpr auto operator()(P&& pl) const noexcept -> UP {
    // Break early if the polyline is empty or has only one segment:
    if (edge_size(pl) < 2) { return pl; }

    UP tmp;
    tmp.reserve(edge_size(pl));

    auto edge_current = edge(pl, 0);

    for (dim_t neidx = 1; neidx < edge_size(pl); ++neidx) {
      auto edge_next = edge(pl, neidx);

      auto r = concatenate_segment_segment(edge_current, edge_next);

      if (r) {
        // segments can be simplified
        edge_current = r.value();

      } else {  // segments cannot be simplified:
        tmp.push_back_edge(edge_current);
        edge_current = edge_next;
      }

      if (neidx == edge_size(pl) - 1) { tmp.push_back_edge(edge_current); }
    }
    return tmp;
  }
};

}  // namespace simplify_polyline_detail

namespace {
static constexpr auto const& simplify_polyline
 = static_const<simplify_polyline_detail::simplify_polyline_fn>::value;
}

}  // namespace hm3::geometry
