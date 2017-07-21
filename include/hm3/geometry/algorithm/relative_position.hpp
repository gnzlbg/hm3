#pragma once
/// \file
///
/// The relative position of one primitive with respect to another.
#include <hm3/geometry/algorithm/relative_position/aabb_line.hpp>
#include <hm3/geometry/algorithm/relative_position/aabb_plane.hpp>
#include <hm3/geometry/algorithm/relative_position/point_line.hpp>
#include <hm3/geometry/algorithm/relative_position/point_plane.hpp>
#include <hm3/geometry/algorithm/relative_position/point_polygon.hpp>
#include <hm3/geometry/algorithm/relative_position/point_polyline.hpp>
#include <hm3/geometry/algorithm/relative_position/point_segment.hpp>
#include <hm3/geometry/algorithm/sd_relative_position.hpp>
#include <hm3/geometry/relative_position.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace relative_position_detail {

struct relative_position_fn {
  template <typename P, typename L>
  static constexpr auto impl(P const& p, L const& l, num_t abs_tol,
                             num_t rel_tol, trait::point<2>,
                             trait::line<2>) noexcept {
    static_assert(Point<P, 2>{});
    static_assert(Line<L, 2>{});
    return relative_position_point_line(p, l, abs_tol, rel_tol);
  }

  template <typename P, typename L, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P const& p, L const& l, num_t abs_tol,
                             num_t rel_tol, trait::point<Ad>,
                             trait::plane<Ad>) noexcept {
    static_assert(ad_v<P> == ad_v<L>);
    static_assert(Point<P>{});
    static_assert(Plane<L>{});
    return relative_position_point_plane(p, l, abs_tol, rel_tol);
  }

  template <typename P, typename L>
  static constexpr auto impl(P const& p, L const& l, num_t abs_tol,
                             num_t rel_tol, trait::point<2>,
                             trait::segment<2>) noexcept {
    static_assert(Point<P, 2>{});
    static_assert(Segment<L, 2>{});
    return relative_position_point_segment(p, l, abs_tol, rel_tol);
  }

  template <typename P, typename L>
  static constexpr auto impl(P const& p, L const& l, num_t abs_tol,
                             num_t rel_tol, trait::point<2>,
                             trait::polyline<2>) noexcept {
    static_assert(Point<P, 2>{});
    static_assert(Polyline<L, 2>{});
    return relative_position_point_polyline(p, l, abs_tol, rel_tol);
  }

  template <typename P, typename A>
  static constexpr auto impl(P const& p, A const& a, num_t abs_tol,
                             num_t rel_tol, trait::point<2>,
                             trait::polygon<2>) noexcept {
    static_assert(Point<P, 2>{});
    static_assert(Polygon<A, 2>{});
    return relative_position_point_polygon(p, a, abs_tol, rel_tol);
  }

  template <typename P, typename L>

  static constexpr auto impl(P const& p, L const& l, num_t abs_tol,
                             num_t rel_tol, trait::aabb<2>,
                             trait::line<2>) noexcept {
    static_assert(AABB<P, 2>{});
    static_assert(Line<L, 2>{});
    return relative_position_aabb_line(p, l, abs_tol, rel_tol);
  }

  template <typename P, typename L, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P const& p, L const& l, num_t abs_tol,
                             num_t rel_tol, trait::aabb<Ad>,
                             trait::plane<Ad>) noexcept {
    static_assert(ad_v<P> == ad_v<L>);
    static_assert(AABB<P>{});
    static_assert(Plane<L>{});
    return relative_position_aabb_plane(p, l, abs_tol, rel_tol);
  }

  template <typename P, typename SD, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P const& p, SD const& sdf, num_t abs_tol,
                             num_t rel_tol, trait::point<Ad>,
                             trait::signed_distance<Ad>) noexcept {
    static_assert(SignedDistance<SD, associated::point_t<P>>{});
    return sd::relative_position(p, sdf, abs_tol, rel_tol);
  }

  template <typename T, typename U>
  static constexpr auto select(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                               fallback)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    invert(impl(std::forward<U>(u), std::forward<T>(t), abs_tol, rel_tol,
                associated::v_<U>, associated::v_<T>)));

  template <typename T, typename U>
  static constexpr auto select(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                               preferred)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             std::forward<U>(u), abs_tol,
                                             rel_tol, associated::v_<T>,
                                             associated::v_<U>));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(select(std::forward<T>(t),
                                               std::forward<U>(u), abs_tol,
                                               rel_tol, dispatch));
};

}  // namespace relative_position_detail

namespace {
constexpr auto const& relative_position = static_const<
 with_default_tolerance<relative_position_detail::relative_position_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
