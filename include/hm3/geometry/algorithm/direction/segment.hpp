#pragma once
/// \file
///
/// Direction of a segment
#include <hm3/geometry/concept/segment.hpp>

namespace hm3::geometry {

namespace direction_segment_detail {

struct direction_segment_fn {
  /// Direction of the line segment \p s (the resulting vector is normalized).
  template <typename T>
  constexpr auto operator()(T&& s) const noexcept {
    static_assert(Segment<uncvref_t<T>>{});
    auto&& d = s.x(1)() - s.x(0)();
    using VT = associated::vector_t<T>;
    return VT(d / d.norm());
  }

  /// Invert direction of line segment \p s.
  template <typename T>
  static constexpr auto invert(T&& s) noexcept {
    static_assert(Segment<uncvref_t<T>>{});
    return uncvref_t<T>{s.x(1), s.x(0)};
  }
};

}  // namespace direction_segment_detail

namespace {
constexpr auto const& direction_segment
 = static_const<direction_segment_detail::direction_segment_fn>::value;
}  // namespace

}  // namespace hm3::geometry
