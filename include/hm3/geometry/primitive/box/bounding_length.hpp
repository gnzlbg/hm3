#pragma once
/// \file
///
/// Bounding lengths of a box.
#include <hm3/geometry/algorithm/bounding_length.hpp>
#include <hm3/geometry/primitive/box/box.hpp>

namespace hm3::geometry::box_primitive {

/// Bounding length of the box \p b along the \p d spatial component.
template <dim_t Nd>
constexpr num_t bounding_length(box<Nd> const& b, dim_t) noexcept {
  return b.length_;
}

/// Bounding lengths of the box \p b along all spatial components.
template <dim_t Nd>  //
constexpr vec<Nd> all_bounding_lengths(box<Nd> const& b) noexcept {
  return vec<Nd>::constant(b.length_);
}

/// Max bounding length of the box \p b.
template <dim_t Nd>  //
constexpr num_t max_bounding_length(box<Nd> const& b) noexcept {
  return b.length_;
}

}  // namespace hm3::geometry::box_primitive
