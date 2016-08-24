#pragma once
/// \file
///
/// Projects a 3D line onto a 2D cartesian plane
#include <hm3/geometry/primitive/line/line.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry::line_primitive {

/// Projects the line \p l onto the Cartesian plane normal to the \p d-th
/// Cartesian direction.
optional<line<2>> project_to_plane(line<3> const& l, dim_t d) noexcept {
  HM3_ASSERT(d < 3, "");
  constexpr dim_t is[3] = {1, 0, 0};
  constexpr dim_t js[3] = {2, 2, 1};
  dim_t i               = is[d];
  dim_t j               = js[d];
  point<2> o            = point<2>{l.origin()(i), l.origin()(j)};
  vec<2> v              = vec<2>{l.direction()(i), l.direction()(j)};
  v                     = v().normalized();
  if (math::approx(v().norm(), 0.)) { return {}; }
  return line<2>(o, v);
}

}  // namespace hm3::geometry::line_primitive
