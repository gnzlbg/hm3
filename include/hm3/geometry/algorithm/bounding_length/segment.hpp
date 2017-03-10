#pragma once
/// \file
///
/// Bounding length of a segment
#include <hm3/geometry/concept/segment.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace bounding_length_segment_detail {

struct bounding_length_segment_fn {
  /// Bounding Length \p d of the segment \p s.
  template <typename P>
  constexpr auto operator()(P&& s, dim_t d) const noexcept {
    static_assert(Segment<uncvref_t<P>>{});
    return std::abs(s.x(0)(d) - s.x(1)(d));
  }

  /// Bounding Length \p d of the segment \p s.
  template <typename P>
  static constexpr auto max(P&& s) noexcept {
    static_assert(Segment<uncvref_t<P>>{});
    return (s.x(0)() - s.x(1)()).array().abs().maxCoeff();
  }

  /// Bounding Length \p d of the segment \p s.
  template <typename P>
  static constexpr auto all(P&& s) noexcept {
    static_assert(Segment<uncvref_t<P>>{});
    using v_t = associated::vector_t<P>;
    return v_t((s.x(0)() - s.x(1)()).array().abs());
  }
};

}  // namespace bounding_length_segment_detail

namespace {
constexpr auto const& bounding_length_segment = static_const<
 bounding_length_segment_detail::bounding_length_segment_fn>::value;
}

}  // namespace hm3::geometry
