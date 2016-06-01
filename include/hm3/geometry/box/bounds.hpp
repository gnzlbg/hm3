#pragma once
/// \file
///
/// Box bounds
#include <hm3/geometry/aabb/bounds.hpp>
#include <hm3/geometry/box/box.hpp>
#include <hm3/geometry/box/centroid.hpp>
#include <hm3/geometry/box/length.hpp>

namespace hm3 {
namespace geometry {

/// Box bounds (min, max)
template <dim_t Nd>
constexpr aabb_bounds<Nd> bounds(box<Nd> const& s) noexcept {
  const auto xc  = centroid(s);
  const num_t l2 = .5 * length(s);
  aabb_bounds<Nd> b;
  b.min() = xc() - point<Nd>::c(l2);
  b.max() = xc() + point<Nd>::c(l2);
  return b;
}

}  // namespace geometry
}  // namespace hm3
