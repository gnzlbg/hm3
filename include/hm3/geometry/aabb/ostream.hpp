#pragma once
/// \file
///
/// AABB Ostream operator
#include <hm3/geometry/aabb/bounds.hpp>
#include <hm3/geometry/aabb/concept.hpp>

namespace hm3 {
namespace geometry {

template <typename OStream, typename Shape, CONCEPT_REQUIRES_(AABB<Shape>{})>
OStream& operator<<(OStream& o, Shape const& s) {
  auto b = bounds(s);
  o << "(center: " << centroid(s) << ", lengths: " << lengths(s)
    << ", min: " << b.min << ", max: " << b.max << ")";
  return o;
}

}  // namespace geometry
}  // namespace hm3
