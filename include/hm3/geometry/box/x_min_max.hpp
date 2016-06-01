#pragma once
/// \file
///
/// Minimum/Maximum vertices
#include <hm3/geometry/box/box.hpp>
#include <hm3/geometry/box/centroid.hpp>
#include <hm3/geometry/box/length.hpp>

namespace hm3 {
namespace geometry {

/// Minimum vertex
template <dim_t Nd> constexpr point<Nd> x_min(box<Nd> const& s) noexcept {
  const auto xc = centroid(s);
  return point<Nd>{xc() - point<Nd>::c(.5 * length(s))};
}

/// Maximum vertex
template <dim_t Nd> constexpr point<Nd> x_max(box<Nd> const& s) noexcept {
  const auto xc = centroid(s);
  return point<Nd>{xc() + point<Nd>::c(.5 * length(s))};
}

}  // namespace geometry
}  // namespace hm3
