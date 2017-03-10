#pragma once
/// \file
///
/// Box concept.
#include <hm3/geometry/concept/ambient_dimension.hpp>
#include <hm3/geometry/concept/geometry_object.hpp>
#include <hm3/geometry/concept/primitive_hierarchy.hpp>

namespace hm3::geometry {

/// Box concept:
///
/// Refines: GeometryObject
///
/// Requires:
///
template <typename T, dim_t Ad = ambient_dimension_v<T>>
using Box
 = meta::and_<GeometryObject<T, Ad, Ad>, trait::check_t<T, trait::box<Ad>>>;

}  // namespace hm3::geometry
