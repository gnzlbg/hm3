#pragma once
/// \file
///
/// Face concept.
#include <hm3/geometry/concept/edge_access.hpp>
#include <hm3/geometry/concept/geometry_object.hpp>
#include <hm3/geometry/concept/line.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace concept {

struct Face
 : rc::refines<GeometryObject, rc::Regular, VertexAccess, EdgeAccess> {
  template <typename T, typename UT = uncvref_t<T>,
            typename VIT = associated::vertex_index_t<UT>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(
   rc::is_true(trait::check<UT, trait::polygon<ambient_dimension_v<UT>>>)  //
   ));
};

}  // namespace concept

/// Face concept:
///
/// Refines: GeometryObject, Regular, EdgeAccess
///
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using Face
 = meta::and_<GeometryObject<T, Ad, 2>, rc::models<concept::Face, T>>;

}  // namespace hm3::geometry
