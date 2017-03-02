#pragma once
/// \file
///
/// Centroid of a box
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace centroid_box_detail {

struct centroid_box_fn {
  /// Centroid of a box
  template <typename P>
  constexpr auto operator()(P&& p) const noexcept {
    static_assert(Same<associated::t_<P>, trait::box<ad_v<P>>>{});
    return p.centroid();
  }
};

}  // namespace centroid_box_detail

namespace {
static constexpr auto const& centroid_box
 = static_const<centroid_box_detail::centroid_box_fn>::value;
}

}  // namespace hm3::geometry
