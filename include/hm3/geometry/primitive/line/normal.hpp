#pragma once
/// \file
///
/// Line normal vector.
#include <hm3/geometry/algorithm/normal.hpp>
#include <hm3/geometry/primitive/line/line.hpp>

namespace hm3::geometry::line_primitive {

/// Counter-clockwise normal vector of the line \p l.
template <dim_t Nd, CONCEPT_REQUIRES_(Nd == 2)>
constexpr vec<Nd> normal(line<Nd> const& l) {
  return normal(l.direction());
}

}  // namespace hm3::geometry::line_primitive
