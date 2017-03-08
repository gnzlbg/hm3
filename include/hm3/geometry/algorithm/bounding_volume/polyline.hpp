#pragma once
/// \file
///
/// Bounding volume of a polyline or polygon.
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/algorithm/x_min_max.hpp>
#include <hm3/geometry/algorithm/bounding_length/aabb.hpp>
#include <hm3/geometry/concept/polygon.hpp>
#include <hm3/geometry/concept/polyhedron.hpp>
#include <hm3/geometry/concept/polyline.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace aabb_of_polyline_detail {

struct aabb_of_polyline_fn {
  /// Aabb_Of of a polyline
  template <typename P>
  constexpr auto operator()(P&& p) const noexcept {
    static_assert(Polyline<uncvref_t<P>>{} or Polygon<uncvref_t<P>>{}
                  or Polyhedron<uncvref_t<P>>{});
    constexpr auto ad = ad_v<P>;
    using aabb_t      = associated::aabb_t<P>;
    using p_t         = associated::point_t<aabb_t>;
    aabb_t bounds;
    x_min(bounds) = p_t::constant(math::highest<num_t>);
    x_max(bounds) = p_t::constant(math::lowest<num_t>);
    for (auto&& v : vertices(p)) {
      for (dim_t d = 0; d < ad; ++d) {
        x_min(bounds)(d) = std::min(x_min(bounds)(d), v(d));
        x_max(bounds)(d) = std::max(x_max(bounds)(d), v(d));
      }
    }

    // Check that we changes the sizes:
    HM3_ASSERT(
     [&]() {
       for (dim_t d = 0; d < ad; ++d) {
         HM3_ASSERT(!approx_number(x_min(bounds)(d), math::highest<num_t>), "");
         HM3_ASSERT(!approx_number(x_max(bounds)(d), math::lowest<num_t>), "");
       }
       return true;
     }(),
     "");

    // A polyline might lie on a Cartesian plane, such that the length of the
    // AABB along that plane (and hence its volume) is zero. This produces a
    // "tight" AABB with non-zero (but very small) volume:
    for (dim_t d = 0; d < ad; ++d) {
      if (approx_number(x_min(bounds)(d), x_max(bounds)(d))) {
        x_min(bounds)(d) = prev_num(x_min(bounds)(d), 2);
        x_max(bounds)(d) = next_num(x_max(bounds)(d), 2);
      }
    }

    bounds.assert_valid();
    return bounds;
  }
};

}  // namespace aabb_of_polyline_detail

namespace {
static constexpr auto const& aabb_of_polyline
 = static_const<aabb_of_polyline_detail::aabb_of_polyline_fn>::value;
static constexpr auto const& aabb_of_polygon
 = static_const<aabb_of_polyline_detail::aabb_of_polyline_fn>::value;
static constexpr auto const& aabb_of_polyhedron
 = static_const<aabb_of_polyline_detail::aabb_of_polyline_fn>::value;
}

namespace box_of_polyline_detail {

struct box_of_polyline_fn {
  /// Square Bounding Box of the polyline \p p.
  template <typename P>
  constexpr auto operator()(P&& p) const noexcept {
    static_assert(Polyline<uncvref_t<P>>{} or Polygon<uncvref_t<P>>{}
                  or Polygon<uncvref_t<P>>{});
    using box_t = associated::box_t<P>;
    auto bounds = aabb_of_polyline(p);
    return box_t{centroid_aabb(bounds), bounding_length_aabb.max(bounds)};
  }
};

}  // namespace box_of_polyline_detail

namespace {
static constexpr auto const& box_of_polyline
 = static_const<box_of_polyline_detail::box_of_polyline_fn>::value;
static constexpr auto const& box_of_polygon
 = static_const<box_of_polyline_detail::box_of_polyline_fn>::value;
static constexpr auto const& box_of_polyhedron
 = static_const<box_of_polyline_detail::box_of_polyline_fn>::value;
}

}  // namespace hm3::geometry
