#pragma once
/// \files
///
/// Types that can be stored in an any geometry.
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/fwd.hpp>
#include <hm3/geometry/primitive/line.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/geometry/primitive/polygon/small_polygon.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/geometry/primitive/polyline/small_polyline.hpp>
#include <hm3/geometry/primitive/ray.hpp>
#include <hm3/geometry/primitive/simplex.hpp>

namespace hm3::geometry::any_primitive {

template <dim_t Nd>
struct polygon {};

template <>
struct polygon<2> {
  using type = geometry::small_polygon<2, 7>;
};

template <>
struct polygon<3> {
  using type = geometry::small_polygon<3, 9>;
};

template <dim_t Nd>
using polygon_t = typename polygon<Nd>::type;

template <dim_t Nd>
struct polyline {};

template <>
struct polyline<2> {
  using type = geometry::small_polyline<2, 4>;
};

template <>
struct polyline<3> {
  using type = geometry::small_polyline<3, 4>;
};

template <dim_t Nd>
using polyline_t = typename polyline<Nd>::type;

template <dim_t Nd>
struct any_types {};

template <>
struct any_types<1> {
  using type = meta::list<point<1>, segment<1>, aabb<1>, box<1>>;
};

template <>
struct any_types<2> {
  using type = meta::list<point<2>, segment<2>, aabb<2>, box<2>, polygon_t<2>,
                          polyline_t<2>>;
};

template <>
struct any_types<3> {
  using type = meta::list<point<3>, segment<3>, aabb<3>, box<3>, polygon_t<3>,
                          polyline_t<3> /*, polyhedra_t<3> */>;
};

template <dim_t Nd>
using any_types_t = typename any_types<Nd>::type;

template <dim_t Nd>
using any_variant_t
 = meta::apply<meta::quote<std::experimental::variant>, any_types_t<Nd>>;

}  // namespace hm3::geometry::any_primitive
