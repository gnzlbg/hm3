#pragma once
/// \file
///
/// Box length
#include <hm3/geometry/box/box.hpp>

namespace hm3 {
namespace geometry {

/// Box length
template <dim_t Nd>
constexpr num_t length(box<Nd> const& s, dim_t = 0) noexcept {
  return s.length_;
}

/// Box lengths (they all equal the same value)
template <dim_t Nd> constexpr point<Nd> lengths(box<Nd> const& s) noexcept {
  return point<Nd>::constant(length(s));
}

}  // namespace geometry
}  // namespace hm3
