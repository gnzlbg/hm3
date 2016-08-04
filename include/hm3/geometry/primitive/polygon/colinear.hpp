#pragma once
/// \file
///
/// Does a polygon have colinear vertices?
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry {

namespace polygon_primitive {

namespace colinear_detail {

struct colinear_fn {
  /// Does the polygon \p p has colinear vertices?
  ///
  /// A polygon has colinear vertices if:
  /// - two consecutive vertices are equal, or
  /// - the direction of two consecutive segments is equal.
  ///
  template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
            CONCEPT_REQUIRES_(Polygon<UP>{})>
  constexpr bool operator()(P&& p) const noexcept {
    auto no_vertices = vertex_size(p);

    auto line_dir = [&](dim_t i, dim_t j) {
      return direction(segment<Nd>::through(vertex(p, i), vertex(p, j)));
    };

    if (vertex(p, 0) == vertex(p, 1)) { return true; }
    auto past_dir = line_dir(0, 1);  // note: directions are always normalized
    HM3_ASSERT(past_dir().norm() == 1, "");
    for (dim_t v = 1; v < no_vertices - 1; ++v) {
      auto new_dir = line_dir(v, v + 1);
      HM3_ASSERT(new_dir().norm() == 1, "");
      if (vertex(p, v) == vertex(p, v + 1) or past_dir == new_dir) {
        return true;
      }
      past_dir = new_dir;
    }

    return false;
  }
};

}  // namespace colinear_detail

namespace {
constexpr auto const& colinear
 = static_const<colinear_detail::colinear_fn>::value;
}  // namespace

}  // namespace polygon_primitive

using polygon_primitive::colinear;

}  // hm3::geometry
