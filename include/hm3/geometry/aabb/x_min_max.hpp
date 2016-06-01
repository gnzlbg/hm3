#pragma once
/// \file
///
/// Rectangle x_min/x_max
#include <hm3/geometry/aabb/aabb.hpp>
namespace hm3 {
namespace geometry {

/// Minimum vertex
template <dim_t Nd>  //
constexpr point<Nd> x_min(aabb<Nd> const& s) noexcept {
  return s.x[0];
}

/// Maximum vertex
template <dim_t Nd>  //
constexpr point<Nd> x_max(aabb<Nd> const& s) noexcept {
  return s.x[1];
}

}  // namespace geometry
}  // namespace hm3
