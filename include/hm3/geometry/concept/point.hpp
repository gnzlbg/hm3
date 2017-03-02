#pragma once
/// \file
///
/// Point concept.
#include <hm3/geometry/concept/fwd.hpp>
#include <hm3/geometry/concept/vertex_access.hpp>

namespace hm3::geometry {

namespace concept {

struct Point : rc::refines<Vertex, VertexAccess> {
  template <typename T>
  auto requires_(T&& t) -> void;
};

}  // namespace concept

/// Point concept.
///
/// Refines Vertex, VertexAccess.
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using Point
 = meta::and_<GeometryObject<T, Ad, 0>, concept::rc::models<concept::Point, T>>;

}  // namespace hm3::geometry
