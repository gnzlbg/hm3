#pragma once
/// \file
///
/// Polygon concept.
#include <hm3/geometry/concept/edge_access.hpp>
#include <hm3/geometry/concept/vertex_access.hpp>

namespace hm3::geometry {

namespace concept {

struct Polygon : rc::refines<VertexAccess, EdgeAccess> {
  template <typename T, typename UT = uncvref_t<T>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(                         //
   rc::is_true(trait::check<UT, trait::polygon<ambient_dimension_v<UT>>>),  //
   // polygons have Ad > 1:
   rc::is_true(meta::bool_<(ambient_dimension_v<UT>> 1)>{}),  //
   ((void)t.polyline(), 42)                                   //
   ));
};

}  // namespace concept

/// Polygon concept:
///
/// Refines: Edge, EdgeAccess
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using Polygon = meta::and_<GeometryObject<T, Ad, 2>,
                           rc::models<concept::Polygon, T>>;

namespace concept {

struct MutablePolygon : rc::refines<Polygon> {
  template <typename T, typename UT = uncvref_t<T>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(  //
   ((void)t.push_back_edge(t.edge(0)), 42),          //
   ((void)t.reserve(0), 42)                          //
   ));
};

}  // namespace concept

/// MutablePolygon:
///
/// Refines: Polygon
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using MutablePolygon
 = meta::and_<GeometryObject<T, Ad, 2>,
              rc::models<concept::MutablePolygon, T>>;

}  // namespace hm3::geometry
