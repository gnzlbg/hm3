#pragma once
/// \file
///
/// Centroid of a box.
#include <hm3/geometry/algorithm/centroid.hpp>
#include <hm3/geometry/primitive/box/box.hpp>

namespace hm3::geometry::box_primitive {

/// Box centroid coordinates
template <dim_t Nd>
constexpr point<Nd> centroid(box<Nd> const& b) noexcept {
  return b.centroid_;
}

}  // namespace hm3::geometry::box_primitive
