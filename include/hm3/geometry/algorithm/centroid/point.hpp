#pragma once
/// \file
///
/// Centroid of a point
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace centroid_point_detail {

struct centroid_point_fn {
  /// Centroid of a point
  template <typename P>
  constexpr decltype(auto) operator()(P&& p) const noexcept {
    static_assert(Point<uncvref_t<P>>{});
    return std::forward<P>(p);
  }
};

}  // namespace centroid_point_detail

namespace {
constexpr auto const& centroid_point
 = static_const<centroid_point_detail::centroid_point_fn>::value;
}

}  // namespace hm3::geometry
