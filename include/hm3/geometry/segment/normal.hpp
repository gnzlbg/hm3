#pragma once
/// \file
///
/// Line segment normal
#include <hm3/geometry/segment/length.hpp>
#include <hm3/geometry/segment/segment.hpp>

namespace hm3 {
namespace geometry {

/// Line segment counter clockwise normal vector
template <dim_t Nd, CONCEPT_REQUIRES_(Nd == 2)>
constexpr point<Nd> normal(segment<Nd> const& l) {
  num_t dx  = l.x_1(0) - l.x_0(0);
  num_t dy  = l.x_1(1) - l.x_0(1);
  num_t len = length(l);
  point<Nd> n;
  n(0) = -dy / len;
  n(1) = dx / len;
  return n;
}

}  // namespace geometry
}  // namespace hm3
