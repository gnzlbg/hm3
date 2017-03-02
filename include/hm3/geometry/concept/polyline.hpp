#pragma once
/// \file
///
/// Polyline concept.
#include <hm3/geometry/concept/edge_access.hpp>
#include <hm3/geometry/concept/vertex_access.hpp>

namespace hm3::geometry {

namespace concept {

struct Polyline : rc::refines<VertexAccess, EdgeAccess> {
  template <typename T, typename UT = uncvref_t<T>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(
   rc::is_true(trait::check<UT, trait::polyline<ambient_dimension_v<UT>>>)  //
   ));
};

}  // namespace concept

/// Polyline concept:
///
/// Refines: Edge, EdgeAccess
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using Polyline = meta::and_<GeometryObject<T, Ad, 1>,
                            concept::rc::models<concept::Polyline, T>>;

namespace concept {

struct MutablePolyline : rc::refines<Polyline> {
  template <typename T, typename UT = uncvref_t<T>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(  //
   ((void)t.push_back_edge(t.edge(0)), 42),          //
   ((void)t.reserve(0), 42)                          //
   ));
};

}  // namespace concept

/// MutablePolyline:
///
/// Refines: Polyline
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using MutablePolyline
 = meta::and_<GeometryObject<T, Ad, 1>,
              concept::rc::models<concept::MutablePolyline, T>>;

}  // namespace hm3::geometry
