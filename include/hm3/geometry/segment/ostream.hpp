#pragma once
/// \file
///
/// Line segment Ostream operator
#include <hm3/geometry/segment/segment.hpp>

namespace hm3 {
namespace geometry {

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, segment<Nd> const& s) {
  auto b = bounds(s);
  o << "(x_0: " << s.x_0 << ", x_1: " << s.x_1 << ")";
  return o;
}

}  // namespace geometry
}  // namespace hm3
