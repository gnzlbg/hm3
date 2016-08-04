#pragma once
/// \file
///
/// Bounding lengths of an AABB.
#include <hm3/geometry/algorithm/bounding_length.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>

namespace hm3::geometry::aabb_primitive {

/// Bounding length of the AABB \p s along the \p d spatial component.
template <dim_t Nd>
constexpr num_t bounding_length(aabb<Nd> const& s, dim_t d) noexcept {
  return x_max(s)(d) - x_min(s)(d);
}

/// Bounding lengths of the AABB \p s along all spatial components.
template <dim_t Nd>  //
constexpr vec<Nd> all_bounding_lengths(aabb<Nd> const& s) noexcept {
  return vec<Nd>(x_max(s)() - x_min(s)());
}

/// Max bounding length of the AABB \p s.
template <dim_t Nd>  //
constexpr num_t max_bounding_length(aabb<Nd> const& s) noexcept {
  return (x_min(s)() - x_max(s)()).array().abs().maxCoeff();
}

}  // namespace hm3::geometry::aabb_primitive
