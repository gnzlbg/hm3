#pragma once
/// \file
///
/// Intersection of line with points.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/primitive/line/line.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::line_primitive {

/// Does the point \p intersect the line \p l?
template <dim_t Nd>
inline bool intersection_test(line<Nd> const& l, point<Nd> const& p) noexcept {
  if (parameter(l, p)) { return true; }
  return false;
}

/// Does the point \p intersect the line \p l?
template <dim_t Nd>
inline variant<monostate, point<Nd>> intersection(line<Nd> const& l,
                                                  point<Nd> const& p) noexcept {
  if (intersection_test(l, p)) { return p; }
  return monostate{};
}

}  // namespace hm3::geometry::line_primitive
