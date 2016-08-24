#pragma once
/// \file
///
/// Bounding lengths of a line segment.
#include <hm3/geometry/algorithm/bounding_length.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::segment_primitive {

/// Bounding length of the line segment \p s along the \p d spatial component.
template <dim_t Nd>
constexpr num_t bounding_length(segment<Nd> const& s, dim_t d) noexcept {
  return std::abs(s.x(0)(d) - s.x(1)(d));
}

/// Bounding lengths of the line segment \p s along all spatial components.
template <dim_t Nd>
constexpr vec<Nd> all_bounding_lengths(segment<Nd> const& s) noexcept {
  return vec<Nd>((s.x(0)() - s.x(1)()).array().abs());
}

/// Max bounding length of the line segment \p s.
template <dim_t Nd>
constexpr num_t max_bounding_length(segment<Nd> const& s) noexcept {
  return (s.x(0)() - s.x(1)()).array().abs().maxCoeff();
}

}  // namespace hm3::geometry::segment_primitive
