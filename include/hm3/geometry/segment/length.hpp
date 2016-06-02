#pragma once
/// \file
///
/// Line segment length
#include <hm3/geometry/segment/segment.hpp>

namespace hm3 {
namespace geometry {

/// Line segment length
template <dim_t Nd> constexpr num_t length(segment<Nd> const& l) {
  return (l.x_1() - l.x_0()).norm();
}

}  // namespace geometry
}  // namespace hm3
