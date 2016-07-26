#pragma once
/// \file
///
/// Output stream operator for a segment.
#include <hm3/geometry/discrete/primitive/segment/segment.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace segment_primitive {

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, segment<Nd> const& s) {
  o << "(x(0): " << s.x(0) << ", x(1): " << s.x(1) << ")";
  return o;
}

}  // namespace segment_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
