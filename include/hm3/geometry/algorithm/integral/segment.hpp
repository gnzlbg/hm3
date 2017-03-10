#pragma once
/// \file
///
/// Integral of a Segment.
#include <hm3/geometry/concept/segment.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace integral_segment_detail {

struct integral_segment_fn {
  /// Path integral of the line segment \p s.
  template <typename T>
  constexpr associated::num_type_t<T> operator()(
   T&& s, trait::path_integral<T>) const {
    static_assert(Segment<uncvref_t<T>>{});
    return (s.x(1)() - s.x(0)()).norm();
  }
};

}  // namespace integral_segment_detail

namespace {

constexpr auto const& integral_segment
 = static_const<integral_segment_detail::integral_segment_fn>::value;
}  // namespace

}  // namespace hm3::geometry
