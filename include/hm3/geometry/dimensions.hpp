#pragma once
/// \file
///
/// Dimension range
#include <hm3/types.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {

/// Range of spatial dimensions: [0, nd)
///
/// TODO: make constexpr when view::iota is constexpr
[[ always_inline, flatten ]] inline auto dimensions(uint_t nd) noexcept {
  return view::iota(0_u, nd);
}
HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(dimensions(1));
HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(dimensions(2));
HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(dimensions(3));

}  // namespace geometry

using geometry::dimensions;

}  // namespace hm3
