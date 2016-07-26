#pragma once
/// \file
///
/// Does the Box contains the point?
#include <hm3/geometry/discrete/algorithm/contains.hpp>
#include <hm3/geometry/discrete/primitive/box/box.hpp>
namespace hm3 {
namespace geometry {
namespace discrete {
namespace box_primitive {

/// Does the Box \p s contains the point \p p?
template <dim_t Nd>
constexpr bool contains(box<Nd> const& s, point<Nd> const& p) noexcept {
  return x_min(s) <= p and x_max(s) >= p;
}

}  // namespace box_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
