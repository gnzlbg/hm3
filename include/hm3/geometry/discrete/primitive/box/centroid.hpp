#pragma once
/// \file
///
/// Centroid of a box.
#include <hm3/geometry/discrete/algorithm/centroid.hpp>
#include <hm3/geometry/discrete/primitive/box/box.hpp>

namespace hm3 {
namespace geometry {

namespace discrete {
namespace box_primitive {

/// Box centroid coordinates
template <dim_t Nd>  //
constexpr point<Nd> centroid(box<Nd> const& b) noexcept {
  return b.centroid_;
}

}  // namespace box_primitive
}  // namespace discrete

}  // namespace geometry
}  // namespace hm3
