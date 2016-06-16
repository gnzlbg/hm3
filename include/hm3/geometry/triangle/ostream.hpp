#pragma once
/// \file
///
/// Triangle Ostream operator
#include <hm3/geometry/triangle/triangle.hpp>

namespace hm3 {
namespace geometry {

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, triangle<Nd> const& s) {
  o << "(v0: " << s.vertices[0] << ", v1: " << s.vertices[1]
    << ", v2: " << s.vertices[2] << ")";
  return o;
}

}  // namespace geometry
}  // namespace hm3
