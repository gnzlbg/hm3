#pragma once
/// \file
///
/// Relative position of point with respect to a line.
#include <hm3/geometry/algorithm/relative_position.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
#include <hm3/geometry/primitive/aabb/vertices.hpp>
#include <hm3/geometry/primitive/box/box.hpp>
#include <hm3/geometry/primitive/box/vertices.hpp>
#include <hm3/geometry/primitive/line/side.hpp>

namespace hm3::geometry::line_primitive {

/// Relative position of the point \p p with respect to the line \p l
inline relative_position_t relative_position(point<2> const& p,
                                             line<2> const& l) noexcept {
  switch (side(l, p)) {
    case side_t::neg: {
      return relative_position_t::outside;
    }
    case side_t::pos: {
      return relative_position_t::inside;
    }
    case side_t::neither: {
      return relative_position_t::intersected;
    }
  }
}

/// Relative position of the AABB \p b with respect to the line \p l
inline relative_position_t relative_position(aabb<2> const& b,
                                             line<2> const& l) noexcept {
  auto rp = relative_position(vertex(b, 0), l);
  for (dim_t vx = 1, e = vertex_size(b); vx != e; ++vx) {
    if (rp != relative_position(vertex(b, vx), l)) {
      return relative_position_t::intersected;
    }
  }
  return rp;
}

/// Relative position of the box \p b with respect to the line \p l
inline relative_position_t relative_position(box<2> const& b,
                                             line<2> const& l) noexcept {
  auto rp = relative_position(vertex(b, 0), l);
  for (dim_t vx = 1, e = vertex_size(b); vx != e; ++vx) {
    if (rp != relative_position(vertex(b, vx), l)) {
      return relative_position_t::intersected;
    }
  }
  return rp;
}

}  // namespace hm3::geometry::line_primitive
