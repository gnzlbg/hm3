#pragma once
#define HM3_USE_EDGE_BASED_POLYLINE
/// \file
///
/// Primitive forward declarations.
#include <hm3/types.hpp>
#include <hm3/utility/small_vector.hpp>

namespace hm3::geometry {

/// \name Fundamental geometric primitives
///@{

namespace point_primitive {
template <dim_t Ad>
struct point;
}  // namespace point_primitive
using point_primitive::point;

namespace vec_primitive {
template <dim_t Ad>
struct vec;
}  // namespace vec_primitive
using vec_primitive::vec;

namespace aabb_primitive {
template <dim_t Ad>
struct aabb;
}  // namespace aabb_primitive
using aabb_primitive::aabb;

namespace box_primitive {
template <dim_t Ad>
struct box;
}  // namespace box_primitive
using box_primitive::box;

namespace line_primitive {
template <dim_t Ad, typename PointT = point<Ad>>
struct line;
}  // namespace line_primitive
using line_primitive::line;

namespace ray_primitive {
template <dim_t Ad>
struct ray;
}  // namespace ray_primitive
using ray_primitive::ray;

namespace segment_primitive {
template <dim_t Ad, typename PT = point<Ad>>
struct segment;
}  // namespace segment_primitive
using segment_primitive::segment;

namespace polyline_primitive {
template <dim_t Ad, typename Storage = small_vector<point<Ad>, 5>>
struct polyline;
}  // namespace polyline_primitive
using polyline_primitive::polyline;

template <dim_t Ad>
using edge_polyline = polyline<Ad, small_vector<segment<Ad>, 4>>;

namespace polygon_primitive {
template <dim_t Ad, typename Storage = small_vector<point<Ad>, 5>>
struct polygon;
}  // namespace polygon_primitive
using polygon_primitive::polygon;

template <dim_t Ad>
using edge_polygon = polygon<Ad, small_vector<segment<Ad>, 4>>;

namespace plane_primitive {
template <dim_t Ad>
struct plane;
}  // namespace plane_primitive
using plane_primitive::plane;

namespace polyhedron_primitive {
template <typename FaceType = polygon<3>>
struct polyhedron;
}  // namespace polyhedron_primitive
using polyhedron_primitive::polyhedron;

///@}  // Fundamental geometric primitives

/// Polymorphic primitive
///@{

namespace some_primitive {
template <dim_t Ad>
struct some;
}
using some_primitive::some;

///@}

/// \name Specialized primitives
///@{

/// Fixed-size polygon
///
/// \tparam Ad Dimension of the ambient space.
/// \tparam Nv Number of polygon vertices.
template <dim_t Ad, dim_t Nv>
using fixed_polygon
 = polygon<Ad, array<point<Ad>, Nv + 1>>;  // TODO: make this == Nv ?

template <dim_t Ad>
using triangle = fixed_polygon<Ad, 3>;
template <dim_t Ad>
using quad = fixed_polygon<Ad, 4>;

namespace simplex_detail {
struct ERROR_UNKNOWN_DIMENSION;
}  // namespace simplex_detail

// clang-format off

/// Simplex: points (1D), segments (2D), triangles(3D)
template <dim_t Ad>
using simplex = std::conditional_t<Ad == 1, point<1>,
                std::conditional_t<Ad == 2, segment<2>,
                std::conditional_t<Ad == 3, triangle<3>,
                simplex_detail::ERROR_UNKNOWN_DIMENSION
               >>>;

// clang-format on

}  // namespace hm3::geometry
