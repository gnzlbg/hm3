#pragma once
/// \file
///
/// Split a polygon with a signed-distance field.
#include <hm3/geometry/algorithm/is_convex.hpp>
#include <hm3/geometry/algorithm/sd_intersection.hpp>
#include <hm3/geometry/algorithm/split.hpp>
#include <hm3/geometry/primitive/polygon.hpp>

namespace hm3::geometry::sd {

namespace split_polygon_detail {

struct split_polygon_fn {
  template <typename P, typename SDF>
  constexpr auto operator()(P&& poly, SDF&& sdf, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    using p_t = associated::point_t<P>;
    static_assert(Polygon<uncvref_t<P>>{});
    static_assert(SignedDistance<uncvref_t<SDF>, p_t>{});

    using r_t = decltype(split.against_range(
     poly, intersection_polygon(poly, sdf, abs_tol, rel_tol).polylines));

    HM3_ASSERT(is_convex(poly),
               "TODO: implement sd::split_polygon for convex polygons");
    // TODO: supporting non-convex polygons in sd::intersection would be enough!

    if (not intersection.test(poly, sdf, abs_tol, rel_tol)) { return r_t{}; }
    auto ir = intersection_polygon(poly, sdf, abs_tol, rel_tol);
    return split.against_range(poly, ir.polylines);
  }
};

}  // namespace split_polygon_detail

namespace {
constexpr auto const& split_polygon = static_const<
 with_default_tolerance<split_polygon_detail::split_polygon_fn>>::value;
}  // namespace

}  // namespace hm3::geometry::sd
