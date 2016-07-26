#pragma once
/// \file
///
/// AABB Ostream operator
#include <hm3/geometry/aabb/bounds.hpp>
#include <hm3/geometry/aabb/concept.hpp>

namespace hm3 {
namespace geometry {

namespace aabb_primitive {

template <typename OStream, typename Shape, CONCEPT_REQUIRES_(AABB<Shape>{})>
OStream& operator<<(OStream& o, Shape const& s) {
  o << "(min: " << x_min(s) << ", max: " << x_max(s) << ")";
  return o;
}

}  // namespace aabb_primitive

}  // namespace geometry
}  // namespace hm3
