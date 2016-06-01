#pragma once
/// \file
///
/// Box centroid
#include <hm3/geometry/box/box.hpp>

namespace hm3 {
namespace geometry {

/// Box centroid coordinates
template <dim_t Nd>  //
constexpr auto centroid(box<Nd> const& s) noexcept {
  return s.centroid_;
}

}  // namespace geometry
}  // namespace hm3
