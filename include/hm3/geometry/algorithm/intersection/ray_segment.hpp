#pragma once
/// \file
///
/// Ray-segment intersection.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>
#include <hm3/geometry/primitive/line/coincidental.hpp>
#include <hm3/geometry/primitive/line/parameter.hpp>
#include <hm3/geometry/primitive/ray/ray.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::ray_primitive {

// Note: these just convert a ray into a "very long" segment (which probably
// will result in floating-point overflow for the current definition of very
// long == math::highest<num_t>). This can probably be made faster and more
// correct by writing custom ray-segment intersection functions (starting from
// the segment-segment ones but checking only for the ray parameter > 0)
//
// For now they work out fine.

namespace ray_detail {

/// Estimate of the ray length when converting it into a segment
template <dim_t Nd>
static constexpr num_t ray_length(point<Nd> const& origin, point<Nd> const& x0,
                                  point<Nd> const& x1) noexcept {
  return 10. + 100. * (origin().norm() + x0().norm() + x1().norm());
}

}  // namespace ray_detail

template <dim_t Nd>
bool intersection_test(ray<Nd> const& a, segment<Nd> const& b) noexcept {
  return intersection.test(
   a.as_segment(ray_detail::ray_length(a.origin(), b.x(0), b.x(1))), b);
}

template <dim_t Nd>
variant<monostate, segment<Nd>, point<Nd>> intersection(
 ray<Nd> const& a, segment<Nd> const& b) noexcept {
  return intersection(
   a.as_segment(ray_detail::ray_length(a.origin(), b.x(0), b.x(1))), b);
}

}  // namespace hm3::geometry::ray_primitive
