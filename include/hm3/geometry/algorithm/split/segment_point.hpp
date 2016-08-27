#pragma once
/// \file
///
/// Split line segment at a point.
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/algorithm/intersection/segment_point.hpp>
#include <hm3/geometry/algorithm/split.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::segment_primitive {

/// Splits the segment \p s at the point \p p.
template <dim_t Nd, typename s_t = segment<Nd>>
constexpr variant<monostate, s_t, pair<s_t, s_t>> split(segment<Nd> const& s,
                                                        point<Nd> const& p) {
  using p_t = point<Nd>;
  using r_t = variant<monostate, s_t, pair<s_t, s_t>>;
  auto ir   = intersection(s, p);

  return visit(
   [&](auto&& v) {
     using T = uncvref_t<decltype(v)>;
     if
       constexpr(Same<T, p_t>{}) {
         if (approx(s.x(0), p) or approx(s.x(1), p)) { return r_t{s}; }
         return r_t{make_pair(s_t(s.x(0), p), s_t(p, s.x(1)))};
       }
     else if
       constexpr(Same<T, monostate>{}) { return r_t{monostate{}}; }
     else {
       static_assert(fail<T>{}, "non-exhaustive visitor");
     }
   },
   ir);
}

}  // namespace hm3::geometry::segment_primitive
