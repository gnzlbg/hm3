#pragma once
/// \file
///
/// Box Ostream operator.
#include <hm3/geometry/discrete/primitive/box/box.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace box_primitive {

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, box<Nd> const& s) {
  o << "(min: " << x_min(s) << ", max: " << x_max(s) << ")";
  return o;
}

}  // namespace box_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
