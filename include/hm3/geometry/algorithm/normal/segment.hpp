#pragma once
/// \file
///
/// Normal of a segment
#include <hm3/geometry/algorithm/direction/segment.hpp>
#include <hm3/geometry/algorithm/normal/vector.hpp>
#include <hm3/geometry/concept/segment.hpp>

namespace hm3::geometry {

namespace normal_segment_detail {

struct normal_segment_fn {
  /// Counter-clockwise normal vector of the segment \p l.
  template <typename T>
  constexpr auto operator()(T&& t) const noexcept {
    static_assert(Segment<uncvref_t<T>, 2>{});
    return normal_vector(direction_segment(t));
  }
};

}  // namespace normal_segment_detail

namespace {
constexpr auto const& normal_segment
 = static_const<normal_segment_detail::normal_segment_fn>::value;
}  // namespace

}  // namespace hm3::geometry
