#pragma once
/// \file
///
/// Integral of a line segment.
#include <hm3/geometry/algorithm/integral.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::segment_primitive {

/// Path integral of the line segment \p s.
template <dim_t Nd>  //
constexpr num_t integral(segment<Nd> const& s,
                         path_integral<segment<Nd>>) noexcept {
  return (s.x(1)() - s.x(0)()).norm();
}

}  // namespace hm3::geometry::segment_primitive
