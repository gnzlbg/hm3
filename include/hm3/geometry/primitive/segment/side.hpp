#pragma once
/// \file
///
/// Side of a point with respect to a segment.
#include <hm3/geometry/algorithm/side.hpp>
#include <hm3/geometry/primitive/line/side.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::segment_primitive {

/// At which side of \p s lies point \p p.
inline side_t side(segment<2> const& s, point<2> const& p) noexcept {
  auto l = s.line();
  return side(l, p);
}

}  // namespace hm3::geometry::segment_primitive
