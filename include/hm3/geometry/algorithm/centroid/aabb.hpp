#pragma once
/// \file
///
/// Centroid of a aabb
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/geometry/algorithm/x_min_max.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace centroid_aabb_detail {

struct centroid_aabb_fn {
  /// Centroid of an AABB
  template <typename P>
  constexpr auto operator()(P&& p) const noexcept {
    static_assert(Same<associated::t_<P>, trait::aabb<ad_v<P>>>{});
    using p_t = associated::point_t<P>;
    return p_t(x_min(p)().array()
               + .5 * (x_max(p)().array() - x_min(p)().array()));
  }
};

}  // namespace centroid_aabb_detail

namespace {
static constexpr auto const& centroid_aabb
 = static_const<centroid_aabb_detail::centroid_aabb_fn>::value;
}

}  // namespace hm3::geometry
