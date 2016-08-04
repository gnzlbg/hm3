#pragma once
/// \file
///
/// Output stream operator for a segment.
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::segment_primitive {

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, segment<Nd> const& s) {
  o << "(x(0): " << s.x(0) << ", x(1): " << s.x(1) << ")";
  return o;
}

}  // namespace hm3::geometry::segment_primitive
