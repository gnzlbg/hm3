#pragma once
/// \file
///
/// Box direction (1D).
#include <hm3/geometry/algorithm/direction.hpp>
#include <hm3/geometry/primitive/box/box.hpp>

namespace hm3::geometry::box_primitive {

/// Length of the 1D box \p s
constexpr vec<1> direction(box<1> const& s) noexcept {
  return vec<1>{s.length_};
}

}  // namespace hm3::geometry::box_primitive
