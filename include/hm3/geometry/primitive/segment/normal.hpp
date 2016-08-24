#pragma once
/// \file
///
/// Line segment normal vector.
#include <hm3/geometry/algorithm/normal.hpp>
#include <hm3/geometry/primitive/segment/integral.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::segment_primitive {

/// Counter-clockwise normal vector of the line segment \p s.
template <dim_t Nd, CONCEPT_REQUIRES_(Nd == 2)>
constexpr vec<Nd> normal(segment<Nd> const& s) {
  return normal(direction(s)).normalized();
}

}  // namespace hm3::geometry::segment_primitive
