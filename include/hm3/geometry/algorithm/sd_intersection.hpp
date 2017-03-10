#pragma once
/// \file
///
/// Intersection of a primitive with the zero-th level of a signed-distance
/// field.
#include <hm3/geometry/algorithm/sd_intersection/polygon.hpp>
#include <hm3/geometry/algorithm/sd_intersection/segment.hpp>
#include <hm3/geometry/algorithm/sd_intersection_test.hpp>

namespace hm3::geometry::sd {

namespace intersection_detail {

struct intersection_fn {
  template <typename S, typename SD, dim_t Ad>
  static constexpr auto impl(S&& s, SD&& sdf, trait::polygon<Ad>, num_t abs_tol,
                             num_t rel_tol) noexcept {
    static_assert(Polygon<uncvref_t<S>, Ad>{});
    static_assert(SignedDistance<uncvref_t<SD>, associated::point_t<S>>{});
    return intersection_polygon(std::forward<S>(s), std::forward<SD>(sdf),
                                abs_tol, rel_tol);
  }

  template <typename S, typename SD, dim_t Ad>
  static constexpr auto impl(S&& s, SD&& sdf, trait::segment<Ad>, num_t abs_tol,
                             num_t rel_tol) noexcept {
    static_assert(Segment<uncvref_t<S>, Ad>{});
    static_assert(SignedDistance<uncvref_t<SD>, associated::point_t<S>>{});
    return intersection_segment(std::forward<S>(s), std::forward<SD>(sdf),
                                abs_tol, rel_tol);
  }

  template <typename T, typename U>
  static constexpr auto intersection_impl(T&& t, U&& u, num_t abs_tol,
                                          num_t rel_tol, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<U>(u),
                                             std::forward<T>(t),
                                             associated::v_<U>, abs_tol,
                                             rel_tol));

  template <typename T, typename U>
  static constexpr auto intersection_impl(T&& t, U&& u, num_t abs_tol,
                                          num_t rel_tol, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             std::forward<U>(u),
                                             associated::v_<T>, abs_tol,
                                             rel_tol));

  template <typename T, typename U>
  static constexpr auto test(T&& t, U&& u, num_t abs_tol, num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_test(std::forward<T>(t),
                                                          std::forward<U>(u),
                                                          abs_tol, rel_tol));

  template <typename T, typename U>
  static constexpr auto test(T&& t, U&& u)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_test(
    std::forward<T>(t), std::forward<U>(u), default_tolerance.absolute(),
    default_tolerance.relative()));

  template <typename Rng>
  static constexpr auto test(Rng&& rng, num_t abs_tol, num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    intersection_test(std::forward<Rng>(rng), abs_tol, rel_tol));

  template <typename Rng>
  static constexpr auto test(Rng&& rng)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(test(std::forward<Rng>(rng),
                                             default_tolerance.absolute(),
                                             default_tolerance.relative()));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_impl(std::forward<T>(t),
                                                          std::forward<U>(u),
                                                          abs_tol, rel_tol, 0));
};

}  // namespace intersection_detail

namespace {
constexpr auto const& intersection = static_const<
 with_default_tolerance<intersection_detail::intersection_fn>>::value;
}  // namespace

}  // namespace hm3::geometry::sd
