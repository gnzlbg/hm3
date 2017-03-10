#pragma once
/// \file
///
/// Does one primitive intersects another?
#include <hm3/geometry/algorithm/intersection/aabb_aabb.hpp>
#include <hm3/geometry/algorithm/intersection/aabb_point.hpp>
#include <hm3/geometry/algorithm/intersection/line_line.hpp>
#include <hm3/geometry/algorithm/intersection/line_point.hpp>
#include <hm3/geometry/algorithm/intersection/line_segment.hpp>
#include <hm3/geometry/algorithm/intersection/point_point.hpp>
#include <hm3/geometry/algorithm/intersection/polygon_point.hpp>
#include <hm3/geometry/algorithm/intersection/polygon_polyline.hpp>
#include <hm3/geometry/algorithm/intersection/polygon_segment.hpp>
#include <hm3/geometry/algorithm/intersection/polyline_point.hpp>
#include <hm3/geometry/algorithm/intersection/ray_aabb.hpp>
#include <hm3/geometry/algorithm/intersection/ray_point.hpp>
#include <hm3/geometry/algorithm/intersection/ray_segment.hpp>
#include <hm3/geometry/algorithm/intersection/segment_aabb.hpp>
#include <hm3/geometry/algorithm/intersection/segment_point.hpp>
#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>

namespace hm3::geometry {

namespace intersection_detail {

struct intersection_fn {
  /// \name point-point
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::point<Ad>,
                                               trait::point<Ad>, num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Point<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return intersection_test_point_point(std::forward<T>(t), std::forward<U>(u),
                                         abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::point<Ad>,
                                          trait::point<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Point<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return intersection_point_point(std::forward<T>(t), std::forward<U>(u),
                                    abs_tol, rel_tol);
  }

  ///@}  // point-point

  /// \name line-point
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::line<Ad>,
                                               trait::point<Ad>, num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Line<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return intersection_test_line_point(std::forward<T>(t), std::forward<U>(u),
                                        abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::line<Ad>,
                                          trait::point<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Line<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return intersection_line_point(std::forward<T>(t), std::forward<U>(u),
                                   abs_tol, rel_tol);
  }

  ///@}  // line-point

  /// \name line-line
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::line<Ad>,
                                               trait::line<Ad>, num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Line<uncvref_t<T>>{});
    static_assert(Line<uncvref_t<U>>{});
    return intersection_test_line_line(std::forward<T>(t), std::forward<U>(u),
                                       abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::line<Ad>,
                                          trait::line<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Line<uncvref_t<T>>{});
    static_assert(Line<uncvref_t<U>>{});
    return intersection_line_line(std::forward<T>(t), std::forward<U>(u),
                                  abs_tol, rel_tol);
  }

  ///@}  // line-line

  /// \name line-segment
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::line<Ad>,
                                               trait::segment<Ad>,
                                               num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Line<uncvref_t<T>>{});
    static_assert(Segment<uncvref_t<U>>{});
    return intersection_test_line_segment(std::forward<T>(t),
                                          std::forward<U>(u), abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::line<Ad>,
                                          trait::segment<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Line<uncvref_t<T>>{});
    static_assert(Segment<uncvref_t<U>>{});
    return intersection_line_segment(std::forward<T>(t), std::forward<U>(u),
                                     abs_tol, rel_tol);
  }

  ///@}  // line-segment

  /// \name ray-point
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::ray<Ad>,
                                               trait::point<Ad>, num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Ray<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return intersection_test_ray_point(std::forward<T>(t), std::forward<U>(u),
                                       abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_non_degenerate_impl(
   T&& t, U&& u, trait::ray<Ad>, trait::point<Ad>, num_t abs_tol,
   num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Ray<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return intersection_test_non_degenerate_ray_point(
     std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::ray<Ad>,
                                          trait::point<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Ray<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return intersection_ray_point(std::forward<T>(t), std::forward<U>(u),
                                  abs_tol, rel_tol);
  }

  ///@}  // ray-point

  /// \name ray-segment
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::ray<Ad>,
                                               trait::segment<Ad>,
                                               num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Ray<uncvref_t<T>>{});
    static_assert(Segment<uncvref_t<U>>{});
    return intersection_test_ray_segment(std::forward<T>(t), std::forward<U>(u),
                                         abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_non_degenerate_impl(
   T&& t, U&& u, trait::ray<Ad>, trait::segment<Ad>, num_t abs_tol,
   num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Ray<uncvref_t<T>>{});
    static_assert(Segment<uncvref_t<U>>{});
    return intersection_test_non_degenerate_ray_segment(
     std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::ray<Ad>,
                                          trait::segment<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Ray<uncvref_t<T>>{});
    static_assert(Segment<uncvref_t<U>>{});
    return intersection_ray_segment(std::forward<T>(t), std::forward<U>(u),
                                    abs_tol, rel_tol);
  }

  ///@}  // ray-segment

  /// \name ray-aabb
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::ray<Ad>,
                                               trait::aabb<Ad>, num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Ray<uncvref_t<T>>{});
    static_assert(AABB<uncvref_t<U>>{});
    return intersection_test_ray_aabb(std::forward<T>(t), std::forward<U>(u),
                                      abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::ray<Ad>,
                                          trait::aabb<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Ray<uncvref_t<T>>{});
    static_assert(AABB<uncvref_t<U>>{});
    return intersection_ray_aabb(std::forward<T>(t), std::forward<U>(u),
                                 abs_tol, rel_tol);
  }

  ///@}  // ray-aabb

  /// \name segment-segment
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::segment<Ad>,
                                               trait::segment<Ad>,
                                               num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Segment<uncvref_t<T>>{});
    static_assert(Segment<uncvref_t<U>>{});
    return intersection_test_segment_segment(
     std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::segment<Ad>,
                                          trait::segment<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Segment<uncvref_t<T>>{});
    static_assert(Segment<uncvref_t<U>>{});
    return intersection_segment_segment(std::forward<T>(t), std::forward<U>(u),
                                        abs_tol, rel_tol);
  }

  ///@}  // segment-segment

  /// \name segment-point
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::segment<Ad>,
                                               trait::point<Ad>, num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Segment<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return intersection_test_segment_point(
     std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::segment<Ad>,
                                          trait::point<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Segment<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return intersection_segment_point(std::forward<T>(t), std::forward<U>(u),
                                      abs_tol, rel_tol);
  }

  ///@}  // segment-point

  /// \name segment-aabb
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::segment<Ad>,
                                               trait::aabb<Ad>, num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Segment<uncvref_t<T>>{});
    static_assert(AABB<uncvref_t<U>>{});
    return intersection_test_segment_aabb(std::forward<T>(t),
                                          std::forward<U>(u), abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::segment<Ad>,
                                          trait::aabb<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Segment<uncvref_t<T>>{});
    static_assert(AABB<uncvref_t<U>>{});
    return intersection_segment_aabb(std::forward<T>(t), std::forward<U>(u),
                                     abs_tol, rel_tol);
  }

  ///@}  // segment-aabb

  /// \name polyline-point
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u,
                                               trait::polyline<Ad>,
                                               trait::point<Ad>, num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Polyline<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return intersection_test_polyline_point(
     std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::polyline<Ad>,
                                          trait::point<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Polyline<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return intersection_polyline_point(std::forward<T>(t), std::forward<U>(u),
                                       abs_tol, rel_tol);
  }

  ///@}  // polyline-point

  /// \name polygon-point
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::polygon<Ad>,
                                               trait::point<Ad>, num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Polygon<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return intersection_test_polygon_point(
     std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::polygon<Ad>,
                                          trait::point<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Polygon<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return intersection_polygon_point(std::forward<T>(t), std::forward<U>(u),
                                      abs_tol, rel_tol);
  }

  ///@}  // polygon-point

  /// \name polygon-segment
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::polygon<Ad>,
                                               trait::segment<Ad>,
                                               num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Polygon<uncvref_t<T>>{});
    static_assert(Segment<uncvref_t<U>>{});
    return intersection_test_polygon_segment(
     std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::polygon<Ad>,
                                          trait::segment<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Polygon<uncvref_t<T>>{});
    static_assert(Segment<uncvref_t<U>>{});
    return intersection_polygon_segment(std::forward<T>(t), std::forward<U>(u),
                                        abs_tol, rel_tol);
  }

  ///@}  // polygon-segment

  /// \name polygon-polyline
  ///@{

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::polygon<Ad>,
                                          trait::polyline<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Polygon<uncvref_t<T>>{});
    static_assert(Polyline<uncvref_t<U>>{});
    return intersection_polygon_polyline(std::forward<T>(t), std::forward<U>(u),
                                         abs_tol, rel_tol);
  }

  ///@}  // polygon-polyline

  /// \name AABB-AABB
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::aabb<Ad>,
                                               trait::aabb<Ad>, num_t,
                                               num_t) noexcept {
    static_assert(Ad == ad_v<U>);
    return intersection_test_aabb_aabb(std::forward<T>(t), std::forward<U>(u));
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::aabb<Ad>,
                                          trait::aabb<Ad>, num_t,
                                          num_t) noexcept {
    static_assert(Ad == ad_v<U>);
    return intersection_aabb_aabb(std::forward<T>(t), std::forward<U>(u));
  }

  ///@}  // AABB-AABB

  /// \name AABB-Point
  ///@{
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_test_impl(T&& t, U&& u, trait::aabb<Ad>,
                                               trait::point<Ad>, num_t abs_tol,
                                               num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Point<uncvref_t<U>>{});
    return intersection_test_aabb_point(std::forward<T>(t), std::forward<U>(u),
                                        abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static auto constexpr intersection_impl(T&& t, U&& u, trait::aabb<Ad>,
                                          trait::point<Ad>, num_t abs_tol,
                                          num_t rel_tol) noexcept {
    static_assert(Ad == ad_v<U>);
    static_assert(Point<uncvref_t<U>>{});
    return intersection_aabb_point(std::forward<T>(t), std::forward<U>(u),
                                   abs_tol, rel_tol);
  }

  ///@}  // AABB-Point

  /// \name Non-degenerate test dispatch
  ///@{

  template <typename T, typename U>
  static constexpr auto intersection_test_non_degenerate_impl(T&& t, U&& u,
                                                              num_t abs_tol,
                                                              num_t rel_tol,
                                                              fallback)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_test_non_degenerate_impl(
    std::forward<U>(u), std::forward<T>(t), associated::v_<U>,
    associated::v_<T>, abs_tol, rel_tol));

  template <typename T, typename U>
  static constexpr auto intersection_test_non_degenerate_impl(T&& t, U&& u,
                                                              num_t abs_tol,
                                                              num_t rel_tol,
                                                              preferred)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_test_non_degenerate_impl(
    std::forward<T>(t), std::forward<U>(u), associated::v_<T>,
    associated::v_<U>, abs_tol, rel_tol));

  ///@}  // Non-degenerate test dispatch

  /// \name Intersection test dispatch
  ///@{

  template <typename T, typename U>
  static constexpr auto intersection_test_impl(T&& t, U&& u, num_t abs_tol,
                                               num_t rel_tol, fallback)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_test_impl(
    std::forward<U>(u), std::forward<T>(t), associated::v_<U>,
    associated::v_<T>, abs_tol, rel_tol));

  template <typename T, typename U>
  static constexpr auto intersection_test_impl(T&& t, U&& u, num_t abs_tol,
                                               num_t rel_tol, preferred)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_test_impl(
    std::forward<T>(t), std::forward<U>(u), associated::v_<T>,
    associated::v_<U>, abs_tol, rel_tol));

  ///@}  // Intersection test dispatch

  /// \name Intersection  dispatch
  ///@{

  template <typename T, typename U>
  static constexpr auto intersection_impl(T&& t, U&& u, num_t abs_tol,
                                          num_t rel_tol, fallback)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    intersection_impl(std::forward<U>(u), std::forward<T>(t), associated::v_<U>,
                      associated::v_<T>, abs_tol, rel_tol));

  template <typename T, typename U>
  static constexpr auto intersection_impl(T&& t, U&& u, num_t abs_tol,
                                          num_t rel_tol, preferred)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    intersection_impl(std::forward<T>(t), std::forward<U>(u), associated::v_<T>,
                      associated::v_<U>, abs_tol, rel_tol));

  ///@}  // Intersection dispatch

  /// \name Intersection test
  ///@{

  template <typename T, typename U>
  static constexpr auto test(T&& t, U&& u, num_t abs_tol, num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_test_impl(
    std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol, hm3::dispatch));

  template <typename T, typename U>
  static constexpr auto test(T&& t, U&& u)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(test(std::forward<T>(t),
                                             std::forward<U>(u),
                                             default_tolerance.absolute(),
                                             default_tolerance.relative()));

  ///@}  // Intersection test

  /// \name Intersection test non-degenerate
  ///@{

  template <typename T, typename U>
  static constexpr auto test_non_degenerate(T&& t, U&& u, num_t abs_tol,
                                            num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_test_non_degenerate_impl(
    std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol, hm3::dispatch));

  template <typename T, typename U>
  static constexpr auto test_non_degenerate(T&& t, U&& u)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(test_non_degenerate(
    std::forward<T>(t), std::forward<U>(u), default_tolerance.absolute(),
    default_tolerance.relative()));

  ///@}  // Intersection test non-degenerate

  /// \name Intersection
  ///@{

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_impl(
    std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol, hm3::dispatch));

  ///@}  // Intersection
};

}  // namespace intersection_detail

namespace {
constexpr auto const& intersection = static_const<
 with_default_tolerance<intersection_detail::intersection_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
