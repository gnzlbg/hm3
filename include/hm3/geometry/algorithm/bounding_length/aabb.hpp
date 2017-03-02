#pragma once
/// \file
///
/// Bounding length of a aabb

#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace bounding_length_aabb_detail {

struct bounding_length_aabb_fn {
  /// Bounding Length \p d of the aabb \p s.
  template <typename P>
  constexpr auto operator()(P&& s, dim_t d) const noexcept {
    static_assert(Same<associated::t_<P>, trait::aabb<ad_v<P>>>{});
    auto res = x_max(s)(d) - x_min(s)(d);
    HM3_ASSERT(math::approx(res, std::abs(res)), "");
    return res;
  }

  /// Bounding Length \p d of the aabb \p s.
  template <typename P>
  static constexpr auto max(P&& s) noexcept {
    constexpr auto ad = ad_v<P>;
    static_assert(Same<associated::t_<P>, trait::aabb<ad>>{});
    // abs not required: max > min!
    auto res = (x_max(s)() - x_min(s)()).array().maxCoeff();
    HM3_ASSERT(
     math::approx(res, (x_max(s)() - x_min(s)()).array().abs().maxCoeff()), "");
    HM3_ASSERT(
     [&]() {
       for (dim_t d = 0; d < ad; ++d) {
         HM3_ASSERT(std::abs(x_max(s)(d) - x_min(s)(d)) <= res, "d: {}", d);
       }
       return true;
     }(),
     "");
    return res;
  }

  /// Bounding Length \p d of the aabb \p s.
  template <typename P>
  static constexpr auto all(P&& s) noexcept {
    static_assert(Same<associated::t_<P>, trait::aabb<ad_v<P>>>{});
    using v_t = associated::vector_t<P>;
    auto res  = v_t(x_max(s)() - x_min(s)());  // abs not required: max > min!
    HM3_ASSERT(approx_vector(res, v_t(res().array().abs())), "");
    return res;
  }
};

}  // namespace bounding length_aabb_detail

namespace {
static constexpr auto const& bounding_length_aabb
 = static_const<bounding_length_aabb_detail::bounding_length_aabb_fn>::value;
}

}  // namespace hm3::geometry
