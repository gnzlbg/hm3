#pragma once
/// \file
///
/// Side of a point with respect to a line.
#include <hm3/geometry/algorithm/side.hpp>
#include <hm3/geometry/primitive/line/line.hpp>

namespace hm3::geometry::line_primitive {

/// At which side of \p l lies point \p p.
inline side_t side(line<2> const& l, point<2> const& p) noexcept {
  auto a  = l.origin();
  auto ab = l.direction();
  auto ap = vec<2>(a() - p());

  auto d = perp_product(ab, ap);

  if (d > 0.) {
    return side_t::neg;
  } else if (d < 0.) {
    return side_t::pos;
  }
  HM3_ASSERT(math::approx(d, 0.), "");
  return side_t::neither;
}

}  // namespace hm3::geometry::line_primitive
