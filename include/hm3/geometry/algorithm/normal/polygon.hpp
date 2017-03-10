#pragma once
/// \file
///
/// Normal of a polygon
#include <hm3/geometry/algorithm/planar_projection.hpp>
#include <hm3/geometry/algorithm/vertex_order.hpp>
#include <hm3/geometry/concept/polygon.hpp>
#include <hm3/geometry/io/ascii.hpp>

namespace hm3::geometry {

namespace normal_polygon_detail {

struct normal_polygon_fn {
  template <typename T>
  static constexpr auto impl(T&& t, trait::polygon<2>) noexcept {
    static_assert(Polygon<uncvref_t<T>, 2>{});
    using v_t   = associated::vector_t<T>;
    using v3d_t = trait::rebind_t<v_t, 3>;
    if (vertex_order(t) == vertex_order_t::ccw) { return v3d_t::unit(2); }
    return v3d_t({0., 0., -1});
  }

  template <typename T>
  static constexpr auto impl(T&& t, trait::polygon<3>) noexcept {
    static_assert(Polygon<uncvref_t<T>, 3>{});
    using v_t = associated::vector_t<T>;
    HM3_ASSERT(
     is_planar(t),
     "non-planar 3D polygon don't have a plane normal vector!\npolygon: {}", t);

    // Get 3 non-colinear polygon vertices:
    auto r = planar_projection_detail::get_three_non_collinear_vertices(t);
    HM3_ASSERT(r, "cannot get three non-collinear vertices of the polygon {}",
               t);
    auto[v0, v1, v2] = r.value();

    // The normal is orthogonal to two vectors in the polygon's plane:
    return v_t((v1() - v0()).cross(v2() - v0()).normalized());
  }

  template <typename T>
  constexpr auto operator()(T&& t) const noexcept {
    static_assert(Polygon<uncvref_t<T>>{});
    return impl(std::forward<T>(t), associated::v_<T>);
  }
};

}  // namespace normal_polygon_detail

namespace {
constexpr auto const& normal_polygon
 = static_const<normal_polygon_detail::normal_polygon_fn>::value;
}  // namespace

}  // namespace hm3::geometry
