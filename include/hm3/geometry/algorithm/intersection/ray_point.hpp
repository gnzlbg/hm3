#pragma once
/// \file
///
/// Intersection of ray with points.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/primitive/ray/ray.hpp>
#include <hm3/geometry/primitive/line/parameter.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::ray_primitive {

/// Does the point \p intersect the ray \p r?
template <dim_t Nd>
bool intersection_test(ray<Nd> const& r, point<Nd> const& p) noexcept {
  if (auto t = parameter(r.as_line(), p)) { return t.value() >= 0.; }
  return false;
}

/// Does the point \p intersect the ray \p r?
template <dim_t Nd>
variant<monostate, point<Nd>> intersection(ray<Nd> const& l,
                                           point<Nd> const& p) noexcept {
  if (intersection_test(l, p)) { return p; }
  return monostate{};
}

}  // namespace hm3::geometry::ray_primitive
