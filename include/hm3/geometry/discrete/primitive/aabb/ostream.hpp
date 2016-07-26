#pragma once
/// \file
///
/// AABB Ostream operator.
#include <hm3/geometry/discrete/primitive/aabb/aabb.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace aabb_primitive {

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, aabb<Nd> const& s) {
  o << "(min: " << x_min(s) << ", max: " << x_max(s) << ")";
  return o;
}

}  // namespace aabb_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
