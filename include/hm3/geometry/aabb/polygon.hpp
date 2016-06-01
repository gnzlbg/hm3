#pragma once
/// \file
///
/// AABB Polygon support
#include <hm3/geometry/aabb/aabb.hpp>
#include <hm3/geometry/polygon/concept.hpp>

namespace hm3 {
namespace geometry {

namespace concepts {

template <> struct is_polygon<geometry::aabb<3>> : std::false_type {};

}  // namespace concepts

}  // namespace geometry
}  // namespace hm3
