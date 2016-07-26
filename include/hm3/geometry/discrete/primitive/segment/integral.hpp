#pragma once
/// \file
///
/// Integral of a line segment.
#include <hm3/geometry/discrete/algorithm/integral.hpp>
#include <hm3/geometry/discrete/primitive/segment/segment.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace segment_primitive {

/// Path integral of the line segment \p s.
template <dim_t Nd>  //
constexpr num_t integral(segment<Nd> const& s,
                         path_integral<segment<Nd>>) noexcept {
  return (s.x(1)() - s.x(0)()).norm();
}

}  // namespace segment_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
