#pragma once
/// \file
///
/// Direction of a line segment.
#include <hm3/geometry/discrete/algorithm/direction.hpp>
#include <hm3/geometry/discrete/primitive/segment/segment.hpp>
namespace hm3 {
namespace geometry {
namespace discrete {
namespace segment_primitive {

/// Direction of the line segment \p s.
template <dim_t Nd>  //
constexpr vector<Nd> direction(segment<Nd> const& s) noexcept {
  auto&& d = s.x(1)() - s.x(0)();
  return vector<Nd>(d / d.norm());
}

/// Invert direction of line segment \p s.
template <dim_t Nd> constexpr segment<Nd> invert_direction(segment<Nd> s) {
  using std::swap;
  swap(s.x(0), s.x(1));
  return s;
}

}  // namespace segment_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
