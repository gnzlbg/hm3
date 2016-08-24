#pragma once
/// \file
///
/// Box integrals.
#include <hm3/geometry/algorithm/integral.hpp>
#include <hm3/geometry/primitive/box/box.hpp>

namespace hm3::geometry::box_primitive {

/// Volume of the box \p s
template <dim_t Nd>
constexpr auto integral(box<Nd> const& s, volume_integral<box<Nd>>) noexcept {
  return math::ipow(s.length_, Nd);
}

/// Boundary integral of the box \p s.
template <dim_t Nd>
constexpr auto integral(box<Nd> const& s, boundary_integral<box<Nd>>) noexcept {
  switch (Nd) {
    case 1: {
      return 2.;  // no_faces * face_surface = 2 * 1
    }
    case 2: {
      return 4. * s.length_;
    }
    case 3: {
      return 6. * math::ipow(s.length_, 2);
    }
    default: { HM3_UNREACHABLE(); }
  }
}

}  // namespace hm3::geometry::box_primitive
