#pragma once
/// \file
///
/// Primitive forward declarations.
#include <hm3/types.hpp>
#include <hm3/utility/small_vector.hpp>

namespace hm3::geometry {

/// \name Fundamental geometric primitives
///@{

namespace point_primitive {
template <dim_t Ad, typename Data = void>
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
template <dim_t Ad, typename PT = point<Ad>, typename Data = void>
struct segment;
}  // namespace segment_primitive
using segment_primitive::segment;

namespace polyline_primitive {

template <dim_t Ad, typename Storage, typename Data>
struct polyline;
}  // namespace polyline_primitive

template <dim_t Ad, typename Point = point<Ad>, typename Data = void>
using polyline = polyline_primitive::polyline<Ad, small_vector<Point, 5>, Data>;

template <dim_t Ad, typename Edge = segment<Ad>, typename Data = void>
using edge_polyline = polyline<Ad, Edge, Data>;

namespace polygon_primitive {
template <dim_t Ad, typename Storage = small_vector<point<Ad>, 5>,
          typename Data = void>
struct polygon;
}  // namespace polygon_primitive

template <dim_t Ad, typename Point = point<Ad>, typename Data = void>
using polygon = polygon_primitive::polygon<Ad, small_vector<Point, 5>, Data>;

template <dim_t Ad, typename Edge = segment<Ad>, typename Data = void>
using edge_polygon = polygon<Ad, Edge, Data>;

namespace plane_primitive {
template <dim_t Ad>
struct plane;
}  // namespace plane_primitive
using plane_primitive::plane;

namespace polyhedron_primitive {
template <typename FaceType = polygon<3>, typename Data = void>
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
///
/// TODO: make this == Nv ?
template <dim_t Ad, dim_t Nv, typename Data = void>
using fixed_polygon
 = polygon_primitive::polygon<Ad, array<point<Ad>, Nv + 1>, Data>;

template <dim_t Ad, typename Data = void>
using triangle = fixed_polygon<Ad, 3, Data>;
template <dim_t Ad, typename Data = void>
using quad = fixed_polygon<Ad, 4, Data>;

namespace simplex_detail {
struct ERROR_UNKNOWN_DIMENSION;
}  // namespace simplex_detail

// clang-format off

/// Simplex: points (1D), segments (2D), triangles(3D)
template <dim_t Ad, typename Data = void>
using simplex = std::conditional_t<Ad == 1, point<1, Data>,
                std::conditional_t<Ad == 2, segment<2, point<2>, Data>,
                std::conditional_t<Ad == 3, triangle<3, Data>,
                simplex_detail::ERROR_UNKNOWN_DIMENSION
               >>>;
// clang-format on

}  // namespace hm3::geometry
