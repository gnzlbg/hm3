#pragma once
/// \file
///
/// Box Ostream operator.
#include <hm3/geometry/primitive/box/box.hpp>

namespace hm3::geometry::box_primitive {

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, box<Nd> const& s) {
  o << "[min: " << x_min(s) << ", max: " << x_max(s) << "]";
  return o;
}

}  // namespace hm3::geometry::box_primitive
