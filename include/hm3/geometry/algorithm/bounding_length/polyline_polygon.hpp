#pragma once
/// \file
///
/// Bounding length of a polyline or polygon.
#include <hm3/geometry/algorithm/bounding_length/aabb.hpp>
#include <hm3/geometry/algorithm/bounding_volume.hpp>
#include <hm3/geometry/algorithm/vertex.hpp>
#include <hm3/geometry/concept/polygon.hpp>
#include <hm3/geometry/concept/polyline.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace bounding_length_polyline_detail {

struct bounding_length_polyline_fn {
  /// Bounding Length \p d of the polyline/polygon \p p.
  template <typename P, typename UP = uncvref_t<P>>
  constexpr auto operator()(P&& p, dim_t d) const noexcept {
    static_assert(Polyline<UP>{} or Polygon<UP>{});
    num_t min_ = 0., max_ = 0.;
    for (auto&& v : vertices(p)) {
      min_ = std::min(min_, v(d));
      max_ = std::max(max_, v(d));
    }
    if (math::approx(min_, max_)) {
      min_ = prev_num(min_, 2);
      max_ = next_num(max_, 2);
    }
    return max_ - min_;
  }

  /// Bounding Length \p d of the polyline \p s.
  template <typename P, typename UP = uncvref_t<P>>
  static constexpr auto max(P&& p) noexcept {
    static_assert(Polyline<UP>{} or Polygon<UP>{});
    return all(p)().array().maxCoeff();
  }

  /// Bounding Length \p d of the polyline \p s.
  template <typename P, typename UP = uncvref_t<P>>
  static constexpr auto all(P&& p) noexcept {
    static_assert(Polyline<UP>{} or Polygon<UP>{});
    return bounding_length_aabb.all(bounding_volume.aabb(p));
  }
};

}  // namespace bounding length_polyline_detail

namespace {
static constexpr auto const& bounding_length_polyline = static_const<
 bounding_length_polyline_detail::bounding_length_polyline_fn>::value;
}

}  // namespace hm3::geometry
