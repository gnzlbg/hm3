#pragma once
/// \file
///
/// AABB volume.
#include <hm3/geometry/algorithm/volume.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
#include <hm3/geometry/primitive/aabb/bounding_length.hpp>

namespace hm3::geometry::aabb_primitive {

/// Volume of the aabb \p s
template <dim_t Nd>  //
constexpr auto volume(aabb<Nd> const& s) noexcept {
  num_t vol    = 1;
  const auto l = all_bounding_lengths(s);
  for (dim_t d = 1; d < Nd; ++d) { vol *= l(d); }
  return vol;
}

}  // namespace hm3::geometry::aabb_primitive
