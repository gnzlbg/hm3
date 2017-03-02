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
// #include <hm3/geometry/algorithm/sd_intersection_test.hpp>
// #include <hm3/geometry/concept/signed_distance.hpp>
#include <hm3/geometry/relative_position.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace relative_position_detail {

template <typename P, typename SDF>
relative_position_t sd_relative_position(P&& p, SDF&& sdf) {
  return sd_intersection_test(std::forward<P>(p), std::forward<SDF>(sdf));
}

struct relative_position_fn {
  template <typename P, typename L,
            CONCEPT_REQUIRES_(Point<P, 2>{} and Line<L, 2>{})>
  static constexpr auto impl(P const& p, L const& l) noexcept {
    return relative_position_point_line(p, l);
  }

  template <typename P, typename L,
            CONCEPT_REQUIRES_(Point<P, 2>{} and Plane<L, 2>{})>
  static constexpr auto impl(P const& p, L const& l) noexcept {
    return relative_position_point_plane(p, l);
  }

  template <typename P, typename L,
            CONCEPT_REQUIRES_(Point<P, 2>{} and Segment<L, 2>{})>
  static constexpr auto impl(P const& p, L const& l) noexcept {
    return relative_position_point_segment(p, l);
  }

  template <typename P, typename L,
            CONCEPT_REQUIRES_(Point<P, 2>{} and Polyline<L, 2>{})>
  static constexpr auto impl(P const& p, L const& l) noexcept {
    return relative_position_point_polyline(p, l);
  }

  template <typename P, typename L,
            CONCEPT_REQUIRES_((Same<associated::t_<P>, trait::box<2>>{}
                               or Same<associated::t_<P>, trait::aabb<2>>{})
                              and Line<L, 2>{})>
  static constexpr auto impl(P const& p, L const& l) noexcept {
    return relative_position_aabb_line(p, l);
  }

  template <typename P, typename L,
            CONCEPT_REQUIRES_((Same<associated::t_<P>, trait::box<2>>{}
                               or Same<associated::t_<P>, trait::aabb<2>>{})
                              and Plane<L, 2>{})>
  static constexpr auto impl(P const& p, L const& l) noexcept {
    return relative_position_aabb_plane(p, l);
  }

  // tries to find an impl, if no impl for the argument order is found, it
  // reverts the order and "inverts" the result (see the definition of invert
  // for how this works):

  template <typename T, typename U>
  static constexpr auto non_sd_dispatch(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(invert(impl(std::forward<U>(u),
                                                    std::forward<T>(t))));

  template <typename T, typename U>
  static constexpr auto non_sd_dispatch(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             std::forward<U>(u)));

  // dispatches to impl of relative position for primitives (no sd fields
  // involved):

  // template <typename T, typename U>
  // static constexpr auto sd_dispatch(T &&t, U &&u, std::false_type)
  //     RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(non_sd_dispatch(std::forward<T>(t),
  //                                                          0,
  //                                                          std::forward<U>(u)));

  // dispatches to impl of relative position w.r.t. signed distance fields:

  // template <typename T, typename U>
  // static constexpr auto sd_dispatch(T &&t, U &&u, std::true_type)
  //     RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
  //         sd_relative_position(std::forward<T>(t), std::forward<U>(u)));

  // template <typename T, typename U>
  // constexpr auto operator()(T &&t, U &&u) const
  //     RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
  //         sd_dispatch(std::forward<T>(t), std::forward<U>(u),
  //                     SignedDistance<U,
  //                     ad_v<uncvref_t<T>>>{}));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(non_sd_dispatch(std::forward<T>(t),
                                                        std::forward<U>(u), 0));
};

}  // namespace relative_position_detail

namespace {
static constexpr auto const& relative_position
 = static_const<relative_position_detail::relative_position_fn>::value;
}  // namespace

}  // namespace hm3::geometry
