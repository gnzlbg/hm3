#pragma once
/// \file
///
/// Polyline concept.
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/concept/polygon.hpp>
#include <hm3/geometry/concept/polyhedron.hpp>

namespace hm3::geometry {

// clang-format off

/// Simplex concept:
///
/// Refines: VertexAccess, EdgeAccess, FaceAcces
template <typename T, dim_t Ad = ambient_dimension_v<T>>
using Simplex
 = meta::and_<GeometryObject<T, Ad, Ad - 1>,
              meta::if_c<Ad == 1, Point<T, 1>,
              meta::if_c<Ad == 2, Segment<T, 2>,
              meta::if_c<Ad == 3, Polygon<T, 3>,
             void>>>>;

// clang-format on

}  // namespace hm3::geometry
