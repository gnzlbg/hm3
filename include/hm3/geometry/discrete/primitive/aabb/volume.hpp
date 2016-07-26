#pragma once
/// \file
///
/// AABB volume.
#include <hm3/geometry/discrete/algorithm/volume.hpp>
#include <hm3/geometry/discrete/primitive/aabb/aabb.hpp>
#include <hm3/geometry/discrete/primitive/aabb/bounding_length.hpp>
namespace hm3 {
namespace geometry {
namespace discrete {
namespace aabb_primitive {

/// Volume of the aabb \p s
template <dim_t Nd>  //
constexpr auto volume(aabb<Nd> const& s) noexcept {
  num_t vol    = 1;
  const auto l = all_bounding_lengths(s);
  for (dim_t d = 1; d < Nd; ++d) { vol *= l(d); }
  return vol;
}

}  // namespace aabb_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
