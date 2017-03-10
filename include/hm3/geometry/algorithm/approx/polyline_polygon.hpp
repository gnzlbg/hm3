#pragma once
/// \file
///
/// Are two polylines/polygons approximately equal
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/algorithm/vertex.hpp>
#include <hm3/geometry/algorithm/x_min_max.hpp>
#include <hm3/geometry/concept/polygon.hpp>
#include <hm3/geometry/concept/polyline.hpp>

namespace hm3::geometry {

namespace approx_polyline_polygon_detail {

template <typename T, typename NT>
constexpr bool approx_polygon_or_polyline(T const& a, T const& b, NT abs_tol,
                                          NT rel_tol) noexcept {
  for (auto r : view::zip(vertices(a), vertices(b))) {
    if (not approx_point(std::get<0>(r), std::get<1>(r), abs_tol, rel_tol)) {
      return false;
    }
  }
  return true;
}

struct approx_polyline_fn {
  /// Generic implementation for polygons are polylines
  template <typename T, typename NT>
  constexpr bool operator()(T const& a, T const& b, NT abs_tol,
                            NT rel_tol) const noexcept {
    static_assert(Polyline<T>{});
    return approx_polygon_or_polyline(a, b, abs_tol, rel_tol);
  }
};

struct approx_polygon_fn {
  /// Generic implementation for polygons are polygons
  template <typename T, typename NT>
  constexpr bool operator()(T const& a, T const& b, NT abs_tol,
                            NT rel_tol) const noexcept {
    static_assert(Polygon<T>{});
    return approx_polygon_or_polyline(a, b, abs_tol, rel_tol);
  }
};

}  // namespace approx_polyline_polygon_detail

namespace {
constexpr auto const& approx_polyline = static_const<with_default_tolerance<
 approx_polyline_polygon_detail::approx_polyline_fn>>::value;

constexpr auto const& approx_polygon = static_const<with_default_tolerance<
 approx_polyline_polygon_detail::approx_polygon_fn>>::value;

}  // namespace

}  // namespace hm3::geometry
