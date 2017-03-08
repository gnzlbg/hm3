#pragma once
/// \file
///
/// The relative position of one primitive with respect to another.
#include <hm3/geometry/algorithm/relative_position/aabb_line.hpp>
#include <hm3/geometry/algorithm/relative_position/aabb_plane.hpp>
#include <hm3/geometry/algorithm/relative_position/point_line.hpp>
#include <hm3/geometry/algorithm/relative_position/point_plane.hpp>
#include <hm3/geometry/algorithm/relative_position/point_polyline.hpp>
#include <hm3/geometry/algorithm/relative_position/point_segment.hpp>
#include <hm3/geometry/algorithm/sd_relative_position.hpp>
#include <hm3/geometry/relative_position.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace relative_position_detail {

struct relative_position_fn {
  template <typename P, typename L,
            CONCEPT_REQUIRES_(Point<P, 2>{} and Line<L, 2>{})>
  static constexpr auto impl(P const& p, L const& l, num_t abs_tol,
                             num_t rel_tol) noexcept {
    return relative_position_point_line(p, l, abs_tol, rel_tol);
  }

  template <typename P, typename L,
            CONCEPT_REQUIRES_(Point<P>{} and Plane<L>{})>
  static constexpr auto impl(P const& p, L const& l, num_t abs_tol,
                             num_t rel_tol) noexcept {
    return relative_position_point_plane(p, l, abs_tol, rel_tol);
  }

  template <typename P, typename L,
            CONCEPT_REQUIRES_(Point<P, 2>{} and Segment<L, 2>{})>
  static constexpr auto impl(P const& p, L const& l, num_t abs_tol,
                             num_t rel_tol) noexcept {
    return relative_position_point_segment(p, l, abs_tol, rel_tol);
  }

  template <typename P, typename L,
            CONCEPT_REQUIRES_(Point<P, 2>{} and Polyline<L, 2>{})>
  static constexpr auto impl(P const& p, L const& l, num_t abs_tol,
                             num_t rel_tol) noexcept {
    return relative_position_point_polyline(p, l, abs_tol, rel_tol);
  }

  template <typename P, typename L,
            CONCEPT_REQUIRES_(AABB<P, 2>{} and Line<L, 2>{})>
  static constexpr auto impl(P const& p, L const& l, num_t abs_tol,
                             num_t rel_tol) noexcept {
    return relative_position_aabb_line(p, l, abs_tol, rel_tol);
  }

  template <typename P, typename L, CONCEPT_REQUIRES_(AABB<P>{} and Plane<L>{})>
  static constexpr auto impl(P const& p, L const& l, num_t abs_tol,
                             num_t rel_tol) noexcept {
    return relative_position_aabb_plane(p, l, abs_tol, rel_tol);
  }

  template <typename P, typename SD,
            CONCEPT_REQUIRES_(SignedDistance<SD, associated::point_t<P>>{})>
  static constexpr auto impl(P const& p, SD const& sdf, num_t abs_tol,
                             num_t rel_tol) noexcept {
    return sd::relative_position(p, sdf, abs_tol, rel_tol);
  }

  template <typename T, typename U>
  static constexpr auto dispatch(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                 long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    invert(impl(std::forward<U>(u), std::forward<T>(t), abs_tol, rel_tol)));

  template <typename T, typename U>
  static constexpr auto dispatch(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                 int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             std::forward<U>(u), abs_tol,
                                             rel_tol));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(dispatch(std::forward<T>(t),
                                                 std::forward<U>(u), abs_tol,
                                                 rel_tol, 0));
};

}  // namespace relative_position_detail

namespace {
static constexpr auto const& relative_position = static_const<
 with_default_tolerance<relative_position_detail::relative_position_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
