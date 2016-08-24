#pragma once
/// \file
///
/// Are two rays parallel?
#include <hm3/geometry/primitive/ray/direction.hpp>
#include <hm3/geometry/primitive/ray/ray.hpp>

namespace hm3::geometry::ray_primitive {

/// Are the rays \p a and \p b parallel?
template <dim_t Nd>
constexpr bool parallel(ray<Nd> const& a, ray<Nd> const& b) noexcept {
  return parallel(direction(a), direction(b));
}

}  // namespace hm3::geometry::ray_primitive
