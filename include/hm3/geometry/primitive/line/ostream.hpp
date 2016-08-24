#pragma once
/// \file
///
/// Output stream operator for a line.
#include <hm3/geometry/primitive/line/line.hpp>

namespace hm3::geometry::line_primitive {

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, line<Nd> const& l) {
  o << "[o: " << l.origin() << ", d: " << l.direction() << "]";
  return o;
}

}  // namespace hm3::geometry::line_primitive
