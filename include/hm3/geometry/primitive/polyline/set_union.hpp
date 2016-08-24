#pragma once
/// \file
///
/// Set union between polylines and segments
#include <hm3/geometry/algorithm/set_union.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry::polyline_primitive {

template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polyline<P, Nd>{})>
constexpr optional<UP> set_union(P l0, P l1) {
  HM3_ASSERT(vertex_size(l0) > 1, "");
  HM3_ASSERT(vertex_size(l1) > 1, "");

  auto first0 = vertex(l0, 0);
  auto last0  = vertex(l0, vertex_size(l0) - 1);

  auto first1 = vertex(l1, 0);
  auto last1  = vertex(l1, vertex_size(l1) - 1);

  if (last0 == first1) {  // insert l1 at the end of l0
    l0.insert(end(l0), begin(l1) + 1, end(l1));
    return l0;
  }
  if (last1 == first0) {  // insert l0 at the end of l1
    l1.insert(end(l1), begin(l0) + 1, end(l0));
    return l1;
  }
  return {};
}

template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polyline<P, Nd>{})>
constexpr optional<UP> set_union(P l, segment<Nd> const& s) {
  HM3_ASSERT(vertex_size(l) > 1, "");

  auto first = first_vertex(l);
  auto last  = last_vertex(l);

  if (s.x(1) == first) {  // push front
    l.insert(begin(l), s.x(0));
    return l;
  }
  if (s.x(0) == last) {  // push back
    l.push_back(s.x(1));
    return l;
  }
  return {};
}

}  // namespace hm3::geometry::polyline_primitive
