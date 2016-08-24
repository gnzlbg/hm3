#pragma once
/// \file
///
/// Direction of a ray.
#include <hm3/geometry/algorithm/direction.hpp>
#include <hm3/geometry/primitive/ray/ray.hpp>

namespace hm3::geometry::ray_primitive {

/// Direction of the ray \p l (the resulting vector is normalized).
template <dim_t Nd>
constexpr vec<Nd> direction(ray<Nd> const& l) noexcept {
  return l.direction();
}

/// Invert direction of ray \p l.
template <dim_t Nd>
constexpr ray<Nd> invert_direction(ray<Nd> l) {
  auto d = l.direction();
  return ray<Nd>(l.origin(), vec<Nd>{-1. * d()});
}

}  // namespace hm3::geometry::ray_primitive
