#pragma once
/// \file
///
/// Distance between two points
#include <hm3/geometry/point/point.hpp>

namespace hm3 {
namespace geometry {

/// Distance between two points
template <dim_t Nd>
num_t distance(point<Nd> const& a, point<Nd> const& b) noexcept {
  return (b() - a()).norm();
}

}  // namespace geometry
}  // namespace hm3
