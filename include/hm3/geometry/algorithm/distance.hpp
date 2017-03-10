#pragma once
/// \file
///
/// Distance computation.
#include <hm3/geometry/algorithm/distance/centroid.hpp>
#include <hm3/geometry/algorithm/distance/minimum_line_line.hpp>
#include <hm3/geometry/algorithm/distance/minimum_line_point.hpp>
#include <hm3/geometry/algorithm/distance/minimum_plane_point.hpp>
#include <hm3/geometry/algorithm/distance/minimum_point_aabb.hpp>
#include <hm3/geometry/algorithm/distance/minimum_point_point.hpp>
#include <hm3/geometry/algorithm/distance/minimum_segment_point.hpp>

namespace hm3::geometry {

namespace distance_detail {

struct distance_fn {
  /// Distance between the centroids of \p t and \p u.
  template <typename T, typename U>
  static constexpr num_t centroid(T&& t, U&& u) noexcept {
    auto const& t_xc = ::hm3::geometry::centroid(std::forward<T>(t));
    auto const& u_xc = ::hm3::geometry::centroid(std::forward<U>(u));
    return distance_fn{}(t_xc, u_xc, default_tolerance.absolute(),
                         default_tolerance.relative());
  }

  template <typename T, typename U,
            CONCEPT_REQUIRES_(Point<T>{} and Point<U>{})>
  static constexpr auto minimum_distance_impl(T const& t, U const& u, num_t,
                                              num_t) noexcept {
    return minimum_distance_point_point(t, u);
  }

  template <typename T, typename U, CONCEPT_REQUIRES_(Line<T>{} and Point<U>{})>
  static constexpr auto minimum_distance_impl(T const& t, U const& u, num_t,
                                              num_t) noexcept {
    return minimum_distance_line_point(t, u);
  }

  template <typename T, CONCEPT_REQUIRES_(Line<T>{})>
  static constexpr auto minimum_distance_impl(T const& t, T const& u,
                                              num_t abs_tol,
                                              num_t rel_tol) noexcept {
    return minimum_distance_line_line(t, u, abs_tol, rel_tol);
  }

  template <typename T, typename P,
            CONCEPT_REQUIRES_(Segment<T>{} and Point<P>{})>
  static constexpr auto minimum_distance_impl(T const& t, P const& p,
                                              num_t abs_tol,
                                              num_t rel_tol) noexcept {
    return minimum_distance_segment_point(t, p, abs_tol, rel_tol);
  }

  template <typename T, typename P,
            CONCEPT_REQUIRES_(Plane<T>{} and Point<P>{})>
  static constexpr auto minimum_distance_impl(T const& t, P const& p, num_t,
                                              num_t) noexcept {
    return minimum_distance_plane_point(t, p);
  }

  template <typename T, typename P, CONCEPT_REQUIRES_(AABB<T>{} and Point<P>{})>
  static constexpr auto minimum_distance_impl(T const& t, P const& p, num_t,
                                              num_t) noexcept {
    return minimum_distance_point_aabb(p, t);
  }

  template <typename T, typename U>
  static constexpr auto minimum_distance_dispatch(T&& t, U&& u, num_t abs_tol,
                                                  num_t rel_tol, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(minimum_distance_impl(
    std::forward<U>(u), std::forward<T>(t), abs_tol, rel_tol));

  template <typename T, typename U>
  static constexpr auto minimum_distance_dispatch(T&& t, U&& u, num_t abs_tol,
                                                  num_t rel_tol, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(minimum_distance_impl(
    std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol));
  /// Minimum distance between two geometry primitivies \p t and\p u.
  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(minimum_distance_dispatch(
    std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol, 0));
};

}  // namespace distance_detail

namespace {
constexpr auto const& distance
 = static_const<with_default_tolerance<distance_detail::distance_fn>>::value;
}

}  // namespace hm3::geometry
