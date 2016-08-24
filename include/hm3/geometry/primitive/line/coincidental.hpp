#pragma once
/// \file
///
/// Direction of a line.
#include <hm3/geometry/primitive/line/line.hpp>

namespace hm3::geometry::line_primitive {

template <dim_t Nd>
constexpr bool coincidental(point<Nd> const& p1, vec<Nd> const& v1,
                            point<Nd> const& p2, vec<Nd> const& v2) noexcept {
  if (!parallel(v1, v2)) { return false; }
  auto w = vec<Nd>(p1() - p2());
  return parallel(w, v2);
}

/// Are the lines \p a and \p b coincidental?
template <dim_t Nd>
constexpr bool coincidental(line<Nd> const& a, line<Nd> const& b) noexcept {
  return coincidental(a.origin(), a.direction(), b.origin(), b.direction());
}

/// Are the lines \p l and \p r coincidental?
bool coincidental(line<1> const&, line<1> const&) noexcept { return true; }

}  // namespace hm3::geometry::line_primitive
