#pragma once
/// \file
///
/// AABB volume
#include <hm3/geometry/aabb/aabb.hpp>
#include <hm3/geometry/aabb/length.hpp>
namespace hm3 {
namespace geometry {

/// Volume of the aabb \p s
template <dim_t Nd>  //
constexpr auto volume(aabb<Nd> const& s) noexcept {
  num_t vol    = 1;
  const auto l = lengths(s);
  for (dim_t d = 1; d < Nd; ++d) { vol *= l(d); }
  return vol;
}

}  // namespace geometry
}  // namespace hm3
