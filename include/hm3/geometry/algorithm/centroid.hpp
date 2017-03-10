#pragma once
/// \file
///
/// Centroid of a geometric primitive.
#include <hm3/geometry/algorithm/centroid/aabb.hpp>
#include <hm3/geometry/algorithm/centroid/box.hpp>
#include <hm3/geometry/algorithm/centroid/point.hpp>
#include <hm3/geometry/algorithm/centroid/polygon.hpp>
#include <hm3/geometry/algorithm/centroid/polyline.hpp>
#include <hm3/geometry/algorithm/centroid/segment.hpp>

namespace hm3::geometry {

namespace centroid_detail {

struct centroid_fn {
  /// \name Default centroid implementations.
  ///@{

  /// Centroid of the point \p p.
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& p, trait::point<Ad>) noexcept {
    static_assert(Point<uncvref_t<P>>{});
    return centroid_point(std::forward<P>(p));
  }

  /// Centroid of the segment \p p.
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& p, trait::segment<Ad>) noexcept {
    static_assert(Segment<uncvref_t<P>>{});
    return centroid_segment(std::forward<P>(p));
  }

  /// Centroid of the polyline \p p.
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& p, trait::polyline<Ad>) noexcept {
    static_assert(Polyline<uncvref_t<P>>{});
    return centroid_polyline(std::forward<P>(p));
  }

  /// Centroid of the polygon \p p.
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& p, trait::polygon<Ad>) noexcept {
    static_assert(Polygon<uncvref_t<P>>{});
    return centroid_polygon(std::forward<P>(p));
  }

  /// Centroid of the aabb \p p.
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& p, trait::aabb<Ad>) noexcept {
    return centroid_aabb(std::forward<P>(p));
  }

  /// Centroid of the box \p p.
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& p, trait::box<Ad>) noexcept {
    return centroid_box(std::forward<P>(p));
  }

  ///@}  // Default centroid implementations

  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             associated::v_<T>));
};

}  // namespace centroid_detail

namespace {
constexpr auto const& centroid
 = static_const<centroid_detail::centroid_fn>::value;
}  // namespace

}  // namespace hm3::geometry
