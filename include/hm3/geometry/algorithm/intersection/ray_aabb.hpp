#pragma once
/// \file
///
/// Does the ray intersect the AABB?
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/algorithm/line_intersection_parameter/ray_aabb.hpp>
#include <hm3/geometry/concept/ray.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace intersection_ray_aabb_detail {

struct intersection_ray_aabb_fn {
  /// Does the AABB \p s and the point \p p intersect?
  template <typename R, typename A>
  constexpr variant<monostate, associated::edge_t<R>, associated::point_t<R>>
   operator()(R const& r, A const& a, num_t abs_tol, num_t rel_tol) const
   noexcept {
    static_assert(Ray<R>{});
    static_assert(AABB<A>{});
    static_assert(ad_v<R> == ad_v<A>);

    using e_t = associated::edge_t<R>;
    using p_t = associated::point_t<R>;
    using r_t = variant<monostate, e_t, p_t>;

    auto i = intersection_parameter_ray_aabb(r, a, abs_tol, rel_tol);
    // no intersection:
    if (not i) { return monostate{}; }

    return visit(
     [&](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if constexpr (Same<T, num_t>{}) {
         // intersection on a point
         return r_t{p_t{r.origin()() + v * r.direction()()}};
       } else {
         // intersection on a line segment:
         static_assert(Same<T, pair<num_t, num_t>>{});
         auto[tmin, tmax] = v;

         p_t p0{r.origin()() + tmin * r.direction()()};
         p_t p1{r.origin()() + tmax * r.direction()()};
         return r_t{e_t{p0, p1}};
       }
     },
     i.value());
  }
};

}  // namespace intersection_ray_aabb_detail

namespace {
constexpr auto const& intersection_ray_aabb
 = static_const<intersection_ray_aabb_detail::intersection_ray_aabb_fn>::value;
}

namespace intersection_test_ray_aabb_detail {

// note, tmax and tmin gives the intersection points of the ray with the aabb.
// If there is an intersection, then:
// - when tmax and tmin are equal, intersection on a point
// - otherwise, the intersection is a segment on the ray ([tmin, tmax]).

struct intersection_test_ray_aabb_fn {
  /// Does the AABB \p s and the point \p p intersect?
  template <typename R, typename A>
  constexpr bool operator()(R const& r, A const& a, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Ray<R>{});
    static_assert(AABB<A>{});
    static_assert(ad_v<R> == ad_v<A>);

    return static_cast<bool>(
     intersection_parameter_ray_aabb(r, a, abs_tol, rel_tol));
  }
};

}  // namespace intersection_test_ray_aabb_detail

namespace {
constexpr auto const& intersection_test_ray_aabb = static_const<
 intersection_test_ray_aabb_detail::intersection_test_ray_aabb_fn>::value;
}

}  // namespace hm3::geometry
