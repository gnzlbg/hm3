#pragma once
/// \file
///
/// Line segment normal vector.
#include <hm3/geometry/discrete/algorithm/normal.hpp>
#include <hm3/geometry/discrete/primitive/segment/integral.hpp>
#include <hm3/geometry/discrete/primitive/segment/segment.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace segment_primitive {

/// Counter-clockwise normal vector of the line segment \p s.
template <dim_t Nd, CONCEPT_REQUIRES_(Nd == 2)>
constexpr point<Nd> normal(segment<Nd> const& s) {
  num_t dx  = s.x(1)(0) - s.x(0)(0);
  num_t dy  = s.x(1)(1) - s.x(0)(1);
  num_t len = length(s);
  point<Nd> n;
  n(0) = -dy / len;
  n(1) = dx / len;
  return n;
}

}  // namespace segment_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
