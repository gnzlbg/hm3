#pragma once
/// \file
///
/// Integral over discrete primitives.
#include <hm3/geometry/algorithm/integral/aabb.hpp>
#include <hm3/geometry/algorithm/integral/box.hpp>
#include <hm3/geometry/algorithm/integral/point.hpp>
#include <hm3/geometry/algorithm/integral/polygon.hpp>
#include <hm3/geometry/algorithm/integral/polyline.hpp>
#include <hm3/geometry/algorithm/integral/segment.hpp>
#include <hm3/geometry/algorithm/integral/vector.hpp>

namespace hm3::geometry {

namespace integral_detail {

struct integral_fn {
  template <typename T, typename K>
  static constexpr auto impl(T const& t, K, trait::point<1>) noexcept {
    static_assert(Point<T, 1>{});
    return integral_point(t, K{});
  }

  template <typename T, typename K, dim_t Ad = ad_v<T>>
  static constexpr auto impl(T const& t, K, trait::vector<Ad>) noexcept {
    static_assert(Vector<T, Ad>{});
    return integral_vector(t, K{});
  }

  template <typename T, typename K, dim_t Ad = ad_v<T>>
  static constexpr auto impl(T const& t, K, trait::segment<Ad>) noexcept {
    static_assert(Segment<T, Ad>{});
    return integral_segment(t, K{});
  }

  template <typename T, typename K, dim_t Ad = ad_v<T>>
  static constexpr auto impl(T const& t, K, trait::polyline<Ad>) noexcept {
    static_assert(Polyline<T>{});
    return integral_polyline(t, K{});
  }

  template <typename T, typename K, dim_t Ad = ad_v<T>>
  static constexpr auto impl(T const& t, K, trait::polygon<Ad>) noexcept {
    static_assert(Polygon<T>{});
    return integral_polygon(t, K{});
  }

  template <typename T, typename K, dim_t Ad = ad_v<T>>
  static constexpr auto impl(T const& t, K, trait::aabb<Ad>) noexcept {
    return integral_aabb(t, K{});
  }

  template <typename T, typename K, dim_t Ad = ad_v<T>>
  static constexpr auto impl(T const& t, K, trait::box<Ad>) noexcept {
    return integral_box(t, K{});
  }

  template <typename T>
  static constexpr auto path(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   impl(std::forward<T>(t), trait::path_integral<T>{}, associated::v_<T>));

  template <typename T>
  static constexpr auto boundary(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   impl(std::forward<T>(t), trait::boundary_integral<T>{}, associated::v_<T>));

  template <typename T>
  static constexpr auto surface(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   impl(std::forward<T>(t), trait::surface_integral<T>{}, associated::v_<T>));

  template <typename T>
  static constexpr auto volume(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   impl(std::forward<T>(t), trait::volume_integral<T>{}, associated::v_<T>));

  template <typename T, typename IntRank>
  constexpr auto operator()(T&& t, IntRank) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t), IntRank{},
                                             associated::v_<T>));
};

}  // namespace integral_detail

namespace {
constexpr auto const& integral
 = static_const<integral_detail::integral_fn>::value;
}  // namespace

namespace integral_detail {

struct area_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(integral.surface(std::forward<T>(t)));
};

}  // namespace integral_detail

namespace {
constexpr auto const& area = static_const<integral_detail::area_fn>::value;
}  // namespace

// namespace integral_detail {

namespace integral_detail {

struct volume_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(integral.volume(std::forward<T>(t)));
};

}  // namespace integral_detail

namespace {
constexpr auto const& volume = static_const<integral_detail::volume_fn>::value;
}  // namespace

namespace integral_detail {

struct length_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(integral.path(std::forward<T>(t)));
};

}  // namespace integral_detail

namespace {
constexpr auto const& length = static_const<integral_detail::length_fn>::value;
}  // namespace

}  // namespace hm3::geometry
