#pragma once
/// \file
///
/// Polyhedron concept.
#include <hm3/geometry/concept/edge_access.hpp>
#include <hm3/geometry/concept/face_access.hpp>
#include <hm3/geometry/concept/vertex_access.hpp>

namespace hm3::geometry {

namespace concept {

struct Polyhedron : rc::refines<VertexAccess, EdgeAccess, FaceAccess> {
  template <typename T, typename UT = uncvref_t<T>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(   //
   rc::is_true(trait::check<UT, trait::polyhedron>),  //
   // polyhedrons have Ad > 1:
   rc::is_true(meta::bool_<(ambient_dimension_v<UT> /* */ > 2)>{})  //
   ));
};

}  // namespace concept

/// Polyhedron concept:
///
/// Refines: VertexAccess, EdgeAccess, FaceAcces
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using Polyhedron = meta::and_<GeometryObject<T, Ad, 3>,
                              rc::models<concept::Polyhedron, T>>;

}  // namespace hm3::geometry
