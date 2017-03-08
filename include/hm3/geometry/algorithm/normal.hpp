#pragma once
/// \file
///
/// Normal
#include <hm3/geometry/algorithm/normal/line.hpp>
#include <hm3/geometry/algorithm/normal/point.hpp>
#include <hm3/geometry/algorithm/normal/polygon.hpp>
#include <hm3/geometry/algorithm/normal/segment.hpp>
#include <hm3/geometry/algorithm/normal/vector.hpp>
#include <hm3/geometry/concept/plane.hpp>

namespace hm3::geometry {

namespace normal_detail {

struct normal_fn {
  template <typename T>
  static constexpr auto impl(T const& t, trait::point<1>) noexcept {
    static_assert(Point<T, 1>{});
    return normal_point(t);
  }

  template <typename T>
  static constexpr auto impl(T const& t, trait::vector<2>) noexcept {
    static_assert(Vector<T, 2>{});
    return normal_vector(t);
  }

  template <typename T>
  static constexpr auto impl(T const& t, trait::line<2>) noexcept {
    static_assert(Line<T, 2>{});
    return normal_line(t);
  }

  template <typename T>
  static constexpr auto impl(T const& t, trait::segment<2>) noexcept {
    static_assert(Segment<T, 2>{});
    return normal_segment(t);
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto impl(T const& t, trait::polygon<Ad>) noexcept {
    static_assert(Polygon<T>{});
    return normal_polygon(t);
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto impl(T const& t, trait::plane<Ad>) noexcept {
    static_assert(Plane<T>{});
    return t.normal();
  }

  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             associated::v_<T>));
};

}  // namespace normal_detail

namespace {
static constexpr auto const& normal
 = static_const<normal_detail::normal_fn>::value;
}  // namespace

}  // namespace hm3::geometry
