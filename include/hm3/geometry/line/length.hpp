#pragma once
/// \file
///
/// Line length
#include <hm3/geometry/line/line.hpp>

namespace hm3 {
namespace geometry {

/// Line length
template <dim_t Nd> constexpr num_t length(line<Nd> const& l) {
  return (l.x_1() - l.x_0()).norm();
}

}  // namespace geometry
}  // namespace hm3
