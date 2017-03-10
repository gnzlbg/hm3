#pragma once
/// \file
///
/// Split one primitive with another
#include <hm3/geometry/algorithm/split/polygon_polyline.hpp>
#include <hm3/geometry/algorithm/split/polyline_point.hpp>
#include <hm3/geometry/algorithm/split/range.hpp>
#include <hm3/geometry/algorithm/split/segment_point.hpp>

namespace hm3::geometry {

namespace split_detail {

struct split_fn {
  /// Split segment-point
  template <typename S, typename P, dim_t Ad = ad_v<S>>
  static constexpr auto impl(S const& s, P const& p, trait::segment<Ad>,
                             trait::point<Ad>) noexcept {
    static_assert(Segment<S>{});
    static_assert(Point<P>{});
    static_assert(ad_v<S> == ad_v<P>);
    return split_segment_point(s, p);
  }

  /// Split polyline-point
  template <typename S, typename P, dim_t Ad = ad_v<S>>
  static constexpr auto impl(S const& s, P const& p, trait::polyline<Ad>,
                             trait::point<Ad>) noexcept {
    static_assert(Polyline<S>{});
    static_assert(Point<P>{});
    static_assert(ad_v<S> == ad_v<P>);
    return split_polyline_point(s, p);
  }

  /// Split polygon-polyline
  template <typename S, typename P, dim_t Ad = ad_v<S>>
  static constexpr auto impl(S const& s, P const& p, trait::polygon<Ad>,
                             trait::polyline<Ad>) noexcept {
    static_assert(Polygon<S>{});
    static_assert(Polyline<P>{});
    static_assert(ad_v<S> == ad_v<P>);
    return split_polygon_polyline(s, p);
  }

  template <typename T, typename U>
  static constexpr auto select(T&& t, U&& u, fallback)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<U>(u),
                                             std::forward<T>(t),
                                             associated::v_<U>,
                                             associated::v_<T>));

  template <typename T, typename U>
  static constexpr auto select(T&& t, U&& u, preferred)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             std::forward<U>(u),
                                             associated::v_<T>,
                                             associated::v_<U>));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(select(std::forward<T>(t),
                                               std::forward<U>(u), dispatch));

  template <typename T, typename Rng>
  static auto against_range(T&& t, Rng&& r) {
    return split_range(std::forward<T>(t), std::forward<Rng>(r), split_fn{});
  }
};

}  // namespace split_detail

namespace {
constexpr auto const& split = static_const<split_detail::split_fn>::value;
}  // namespace

}  // namespace hm3::geometry
