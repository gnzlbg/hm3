#pragma once
#include <hm3/geometry/primitive/polyline/faces.hpp>

namespace hm3::geometry::polyline_primitive {

template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polyline<P, Nd>{})>
constexpr auto insert_vertex(P& pl, suint_t sidx, point<Nd> const& p) noexcept
 -> void {
  HM3_ASSERT(sidx < face_size(pl), "segment index {} out of bounds [, {})",
             sidx, face_size(pl));
  HM3_ASSERT(pl.size() < pl.max_size(), "polyline size {} !< max size {} ",
             pl.size(), pl.max_size());
  pl.insert(begin(pl) + sidx + 1, p);
}

}  // namespace hm3::geometry::polyline_primitive
