#pragma once
/// \file
///
/// Bounding length of a box
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace bounding_length_box_detail {

struct bounding_length_box_fn {
  /// Bounding Length \p d of the box \p s.
  template <typename P>
  constexpr auto operator()(P&& s, dim_t) const noexcept {
    static_assert(Same<associated::t_<P>, trait::box<ad_v<P>>>{});
    return s.length();
  }

  /// Bounding Length \p d of the box \p s.
  template <typename P>
  static constexpr auto max(P&& s) noexcept {
    static_assert(Same<associated::t_<P>, trait::box<ad_v<P>>>{});
    return bounding_length_box_fn{}(s, 0);
  }

  /// Bounding Length \p d of the box \p s.
  template <typename P>
  static constexpr auto all(P&& s) noexcept {
    static_assert(Same<associated::t_<P>, trait::box<ad_v<P>>>{});
    using v_t = associated::vector_t<P>;
    return v_t::constant(bounding_length_box_fn{}(s, 0));
  }
};

}  // namespace bounding_length_box_detail

namespace {
constexpr auto const& bounding_length_box
 = static_const<bounding_length_box_detail::bounding_length_box_fn>::value;
}

}  // namespace hm3::geometry
