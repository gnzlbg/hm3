#pragma once
/// \file
///
/// Direction
#include <hm3/geometry/algorithm/direction/aabb_box.hpp>
#include <hm3/geometry/algorithm/direction/line.hpp>
#include <hm3/geometry/algorithm/direction/polyline.hpp>
#include <hm3/geometry/algorithm/direction/segment.hpp>
#include <hm3/geometry/algorithm/direction/vector.hpp>

namespace hm3::geometry {

namespace direction_detail {

struct direction_fn {
  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto direction_impl(T const& t, trait::vector<Ad>) {
    static_assert(Vector<T>{});
    return direction_vector(t);
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto invert_impl(T const& t, trait::vector<Ad>) {
    static_assert(Vector<T>{});
    return direction_vector.invert(t);
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto direction_impl(T const& t, trait::line<Ad>) {
    static_assert(Line<T>{});
    return direction_line(t);
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto invert_impl(T const& t, trait::line<Ad>) {
    static_assert(Line<T>{});
    return direction_line.invert(t);
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto direction_impl(T const& t, trait::ray<Ad>) {
    static_assert(Ray<T>{});
    return direction_line(t);
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto invert_impl(T const& t, trait::ray<Ad>) {
    static_assert(Ray<T>{});
    return direction_line.invert(t);
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto direction_impl(T const& t, trait::segment<Ad>) {
    static_assert(Segment<T>{});
    return direction_segment(t);
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto invert_impl(T const& t, trait::segment<Ad>) {
    static_assert(Segment<T>{});
    return direction_segment.invert(t);
  }

  // note: direction for polylines does not make sense (explicitly ommitted)

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto invert_impl(T const& t, trait::polyline<Ad>) {
    static_assert(Polyline<T>{});
    return direction_polyline.invert(t);
  }

  template <typename T>
  static constexpr auto direction_impl(T const& t, trait::aabb<1>) {
    return direction_aabb_box(t);
  }

  template <typename T>
  static constexpr auto direction_impl(T const& t, trait::box<1>) {
    return direction_aabb_box(t);
  }

  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(direction_impl(std::forward<T>(t),
                                                       associated::v_<T>));

  template <typename T>
  static constexpr auto invert(T&& t)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(invert_impl(std::forward<T>(t),
                                                    associated::v_<T>));

  template <typename A, typename P>
  constexpr static auto closest(A a, P p) noexcept {
    using vec_t         = associated::vector_t<P>;
    auto x_c            = centroid(a);
    auto d              = p() - x_c();
    sint_t max_distance = -1;
    d.array().abs().maxCoeff(&max_distance);
    vec_t v = vec_t::unit(max_distance);
    HM3_ASSERT(max_distance >= 0 and max_distance < (sint_t)ad_v<P>, "");
    if (d(max_distance) > 0.) { return v; }
    return invert(v);
  }
};

}  // namespace direction_detail

namespace {
static constexpr auto const& direction
 = static_const<direction_detail::direction_fn>::value;
}  // namespace

}  // namespace hm3::geometry
