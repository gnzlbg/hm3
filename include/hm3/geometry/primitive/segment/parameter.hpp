#pragma once
/// \file
///
/// Line parameter from point in segment
#include <hm3/geometry/algorithm/approx.hpp>
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

/// Value of the segment parameter if the point \p lies in the line of the
/// segment \p s such that for `p == s.x(0) => t =0` and `p == s.x(1) => t = 1`.
///
/// If \p in_segment is true, returns nothing if the points lies outside the
/// segment. Otherwise it returns nothing if the point does not lie on the line
/// spanned by the segment, and the line parameter otherwise.
template <dim_t Nd>
constexpr optional<num_t> parameter(segment<Nd> const& s, point<Nd> const& p,
                                    bool in_segment = true) noexcept {
  num_t dist
   = in_segment ? distance.minimum(s, p) : distance.minimum(s.line(), p);
  if (geometry::approx(dist, 0.)) {
    const auto dir = direction(s);
    for (dim_t d = 0; d < Nd; ++d) {
      if (!math::approx(dir(d), 0.)) { return parameter_detail::t(s, p, d); }
    }
    HM3_ASSERT(false, "segment: {}, direction vector has norm zero", s);
  }
  return {};
}

}  // namespace hm3::geometry::segment_primitive
