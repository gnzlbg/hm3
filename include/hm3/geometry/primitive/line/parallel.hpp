#pragma once
/// \file
///
/// Are two lines parallel?
#include <hm3/geometry/primitive/line/direction.hpp>
#include <hm3/geometry/primitive/line/line.hpp>

namespace hm3::geometry::line_primitive {

/// Are the lines \p a and \p b parallel?
template <dim_t Nd>
constexpr bool parallel(line<Nd> const& a, line<Nd> const& b) noexcept {
  return parallel(direction(a), direction(b));
}

}  // namespace hm3::geometry::line_primitive
