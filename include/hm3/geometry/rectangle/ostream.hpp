#pragma once
/// \file
///
/// Rectangle ostream operator
#include <hm3/geometry/rectangle/concept.hpp>

namespace hm3 {
namespace geometry {

template <typename OStream, typename Shape,
          CONCEPT_REQUIRES_(Rectangle<Shape>{})>
OStream& operator<<(OStream& o, Shape const& s) {
  auto b = bounds(s);
  o << "(center: " << centroid(s) << ", lengths: " << lengths(s)
    << ", min: " << b.min << ", max: " << b.max << ")";
  return o;
}

}  // namespace geometry
}  // namespace hm3
