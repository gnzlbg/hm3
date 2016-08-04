#pragma once
/// \file
///
/// Does the Box contains the point?
#include <hm3/geometry/algorithm/contains.hpp>
#include <hm3/geometry/primitive/box/box.hpp>

namespace hm3::geometry::box_primitive {

/// Does the Box \p s contains the point \p p?
template <dim_t Nd>
constexpr bool contains(box<Nd> const& s, point<Nd> const& p) noexcept {
  return x_min(s) <= p and x_max(s) >= p;
}

}  // namespace hm3::geometry::box_primitive
