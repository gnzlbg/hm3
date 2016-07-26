#pragma once
/// \file
///
/// Box direction (1D).
#include <hm3/geometry/discrete/algorithm/direction.hpp>
#include <hm3/geometry/discrete/primitive/box/box.hpp>
namespace hm3 {
namespace geometry {
namespace discrete {
namespace box_primitive {

/// Length of the 1D box \p s
constexpr vector<1> direction(box<1> const& s) noexcept {
  return vector<1>{s.length_};
}

}  // namespace box_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
