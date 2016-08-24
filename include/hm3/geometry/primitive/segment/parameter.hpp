#pragma once
/// \file
///
/// Line parameter from point in segment
#include <hm3/geometry/primitive/segment/distance.hpp>
#include <hm3/geometry/primitive/segment/ostream.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry::segment_primitive {

namespace parameter_detail {

template <dim_t Nd>
constexpr num_t t(segment<Nd> const& l, point<Nd> const& p, dim_t d) noexcept {
  auto denom = l.x(1)(d) - l.x(0)(d);
  HM3_ASSERT(!math::approx(std::abs(denom), 0.),
             "denom is zero (l: {}, p: {}, d: {})", l, p, d);
  return (p(d) - l.x(0)(d)) / denom;
}

}  // namespace parameter_detail

/// Either value of the line parameter if the point \p lies in the line \p l
/// or nothing.
template <dim_t Nd>
constexpr optional<num_t> parameter(segment<Nd> const& l,
                                    point<Nd> const& p) noexcept {
  if (distance.approx(distance.minimum(l, p), 0.)) {
    const auto dir = direction(l);
    for (dim_t d = 0; d < Nd; ++d) {
      if (!math::approx(dir(d), 0.)) { return parameter_detail::t(l, p, d); }
    }
    HM3_ASSERT(false, "line: {}, direction vector has norm zero", l);
  }
  return {};
}

}  // namespace hm3::geometry::segment_primitive
