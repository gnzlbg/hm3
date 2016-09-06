#pragma once
/// \file
///
/// Set union between non-overlapping segments.
///
/// TODO: test for overlap!
#include <hm3/geometry/algorithm/set_union.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry::segment_primitive {

template <dim_t Nd, typename R = small_polyline<Nd, 2>>
constexpr optional<R> set_union(segment<Nd> const& s0, segment<Nd> const& s1) {
  if (s1.x(1) == s0.x(0)) {  // push front
    return {s1.x(0), s1.x(1), s0.x(1)};
  }
  if (s1.x(0) == s0.x(1)) {  // push back
    return {s0.x(0), s0.x(1), s1.x(1)};
  }
  return {};
}

}  // namespace hm3::geometry::segment_primitive
