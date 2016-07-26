#pragma once
/// \file
///
/// Bounding lengths of a line segment.
#include <hm3/geometry/discrete/algorithm/bounding_length.hpp>
#include <hm3/geometry/discrete/primitive/segment/segment.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace segment_primitive {

/// Bounding length of the line segment \p s along the \p d spatial component.
template <dim_t Nd>
constexpr num_t bounding_length(segment<Nd> const& s, dim_t d) noexcept {
  return std::abs(s.x(0)(d) - s.x(1)(d));
}

/// Bounding lengths of the line segment \p s along all spatial components.
template <dim_t Nd>  //
constexpr vector<Nd> all_bounding_lengths(segment<Nd> const& s) noexcept {
  return vector<Nd>((s.x(0)() - s.x(1)()).array().abs());
}

/// Max bounding length of the line segment \p s.
template <dim_t Nd>  //
constexpr num_t max_bounding_length(segment<Nd> const& s) noexcept {
  return (s.x(0)() - s.x(1)()).array().abs().maxCoeff();
}

}  // namespace segment_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
