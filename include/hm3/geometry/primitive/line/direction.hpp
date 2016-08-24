#pragma once
/// \file
///
/// Direction of a line.
#include <hm3/geometry/algorithm/direction.hpp>
#include <hm3/geometry/primitive/line/line.hpp>

namespace hm3::geometry::line_primitive {

/// Direction of the line \p l (the resulting vector is normalized).
template <dim_t Nd>
constexpr vec<Nd> direction(line<Nd> const& l) noexcept {
  return l.direction();
}

/// Invert direction of line \p l.
template <dim_t Nd>
constexpr line<Nd> invert_direction(line<Nd> l) {
  return line<Nd>(l.origin(), vec<Nd>{-1. * l.direction()()});
}

}  // namespace hm3::geometry::line_primitive
