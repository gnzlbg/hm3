#pragma once
/// \file
///
/// Parameter of the intersection of a point with a line.
#include <hm3/ext/variant.hpp>
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/algorithm/x_min_max.hpp>
#include <hm3/geometry/concept/aabb.hpp>
#include <hm3/geometry/concept/ray.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry {

namespace intersection_parameter_ray_aabb_detail {

struct intersection_parameter_ray_aabb_fn {
  /// Intersection parameter between a ray and an AABB:
  ///
  /// If there is an intersection, then:
  /// - when tmax and tmin are equal, intersection on a point
  /// - otherwise, the intersection is a segment on the ray ([tmin, tmax]).
  template <typename R, typename A>
  optional<variant<pair<num_t, num_t>, num_t>> operator()(R&& r, A&& a,
                                                          num_t abs_tol,
                                                          num_t rel_tol) const
   noexcept {
    static_assert(Ray<uncvref_t<R>>{});
    static_assert(AABB<uncvref_t<A>>{});
    static_assert(ad_v<R> == ad_v<A>);

    using r_t = variant<pair<num_t, num_t>, num_t>;

    HM3_ASSERT(not approx_number(r.direction()().norm(), 0.),
               "ray: {}, direction vector has norm zero", r);

    // note: d_inv can contain NaNs 1./0., this is fine and is handled later:
    auto d_inv = r.direction()().array().inverse().eval();

    auto tx1 = ((x_min(a)() - r.origin()()).array() * d_inv).eval();
    auto tx2 = ((x_max(a)() - r.origin()()).array() * d_inv).eval();

    num_t tmin = math::lowest<num_t>;
    num_t tmax = math::highest<num_t>;
    for (dim_t d = 0; d < ad_v<A>; ++d) {
      // if d_inv contains a NaN, we skip it.
      if (not approx_number(r.direction()(d), 0.0, abs_tol, rel_tol)) {
        tmin = std::max(tmin, std::min(tx1(d), tx2(d)));
        tmax = std::min(tmax, std::max(tx1(d), tx2(d)));
      }
    }

    // If the ray intersects, return the intersection parameters:
    if (tmax > 0 or approx_number(tmax, 0., abs_tol, rel_tol)) {
      // intersection on a point
      if (approx_number(tmax, tmin, abs_tol, rel_tol)) { return r_t{tmin}; }
      // intersection on a line segment
      if (tmax > tmin) { return r_t{make_pair(std::max(tmin, 0.0), tmax)}; }
    }

    // Otherwise return nothing:
    return {};
  }
};

}  // namespace intersection_parameter_ray_aabb_detail

namespace {
constexpr auto const& intersection_parameter_ray_aabb
 = static_const<with_default_tolerance<
  intersection_parameter_ray_aabb_detail::intersection_parameter_ray_aabb_fn>>::
  value;
}  // namespace

}  // namespace hm3::geometry
