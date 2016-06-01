#pragma once
/// \file
///
/// AABB length
#include <hm3/geometry/aabb/aabb.hpp>
#include <hm3/geometry/aabb/x_min_max.hpp>
namespace hm3 {
namespace geometry {

/// Length of the AABB \p s along the \p d spatial component
template <dim_t Nd>
constexpr num_t length(aabb<Nd> const& s, dim_t d) noexcept {
  return x_max(s)(d) - x_min(s)(d);
}

/// lengths of the AABB along all spatial components
template <dim_t Nd>  //
constexpr vector<Nd> lengths(aabb<Nd> const& s) noexcept {
  const auto b = bounds(s);
  return vector<Nd>(b.max() - b.min());
}

}  // namespace geometry
}  // namespace hm3
