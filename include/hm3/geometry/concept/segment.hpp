#pragma once
/// \file
///
/// Segment concept.
#include <hm3/geometry/concept/edge.hpp>
#include <hm3/geometry/concept/edge_access.hpp>

namespace hm3::geometry {

namespace concept {

struct Segment : rc::refines<Edge, EdgeAccess, VertexAccess> {
  template <typename T, typename UT = uncvref_t<T>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(
   rc::is_true(trait::check<UT, trait::segment<ambient_dimension_v<UT>>>)  //
   ));
};

}  // namespace concept

/// Segment concept:
///
/// Refines: Edge, EdgeAccess
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using Segment = meta::and_<GeometryObject<T, Ad, 1>,
                           rc::models<concept::Segment, T>>;

}  // namespace hm3::geometry
