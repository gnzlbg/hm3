#pragma once
/// \file
///
/// AABB integral.
#include <hm3/geometry/algorithm/integral.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
#include <hm3/geometry/primitive/aabb/bounding_length.hpp>

namespace hm3::geometry::aabb_primitive {

/// Integral over the volume of the AABB \p s.
template <dim_t Nd>  //
constexpr auto integral(aabb<Nd> const& s, volume_integral<aabb<Nd>>) noexcept {
  num_t vol    = 1;
  const auto l = all_bounding_lengths(s);
  for (dim_t d = 1; d < Nd; ++d) { vol *= l(d); }
  return vol;
}

/// Integral over the volume of the 1D AABB \p s.
constexpr num_t integral(aabb<1> const& s, volume_integral<aabb<1>>) noexcept {
  return bounding_length(s, 0);
}

/// Integral over the boundary of the AABB \p s.
template <dim_t Nd>  //
constexpr auto integral(aabb<Nd> const& s,
                        boundary_integral<aabb<Nd>>) noexcept {
  switch (Nd) {
    case 1: {
      return 2.;  // no_faces * area of faces == 2 * 1
    }
    case 2: {
      const auto ls = all_bounding_lengths(s);
      return 2. * (ls(0) + ls(1));
    }
    case 3: {
      const auto ls   = all_bounding_lengths(s);
      const auto lxly = ls(0) * ls(1);
      const auto lylz = ls(1) * ls(2);
      const auto lxlz = ls(0) * ls(2);
      return 2. * (lxly + lylz + lxlz);
    }
    default: { HM3_UNREACHABLE(); }
  }
}

}  // namespace hm3::geometry::aabb_primitive
