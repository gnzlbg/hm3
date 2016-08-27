#pragma once
/// \file
///
/// Primitive forward declarations.
#include <hm3/types.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/inline_vector.hpp>
#include <hm3/utility/small_vector.hpp>

namespace hm3::geometry {

namespace point_primitive {
template <dim_t Nd>
struct point;
}  // namespace point_primitive
using point_primitive::point;

namespace vec_primitive {
template <dim_t Nd>
struct vec;
}  // namespace vec_primitive
using vec_primitive::vec;

namespace aabb_primitive {
template <dim_t Nd>
struct aabb;
}  // namespace aabb_primitive
using aabb_primitive::aabb;

namespace box_primitive {
template <dim_t Nd>
struct box;
}  // namespace box_primitive
using box_primitive::box;

namespace line_primitive {
template <dim_t Nd>
struct line;
}  // namespace segment_primitive
using line_primitive::line;

namespace ray_primitive {
template <dim_t Nd>
struct ray;
}  // namespace ray_primitive

namespace segment_primitive {
template <dim_t Nd>
struct segment;
}  // namespace segment_primitive
using segment_primitive::segment;

namespace polygon_primitive {

template <dim_t Nd, typename Storage>
struct polygon;

/// Small polygon.
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam EstNp Maximum expected number of vertices that the polygon
/// can contain.
template <dim_t Nd, dim_t EstNp>
using small_polygon = polygon<Nd, small_vector<point<Nd>, EstNp>>;

/// Fixed polygon.
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam Nv Number of vertices.
template <dim_t Nd, dim_t Nv>
using fixed_polygon = polygon<Nd, array<point<Nd>, Nv>>;

/// Bounded polygon
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam MaxNv Maximum number of vertices that the polygon can contain.
template <dim_t Nd, dim_t MaxNv>
using bounded_polygon = polygon<Nd, inline_vector<point<Nd>, MaxNv>>;

}  // namespace polygon_primitive
using polygon_primitive::bounded_polygon;
using polygon_primitive::small_polygon;
using polygon_primitive::fixed_polygon;
template <dim_t Nd>
using triangle = fixed_polygon<Nd, 3>;
template <dim_t Nd>
using quad = fixed_polygon<Nd, 4>;

namespace polyline_primitive {
template <dim_t Nd, typename Storage>
struct polyline;

/// Small polyline.
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam EstNp Maximum expected number of vertices that the polyline
/// can contain.
template <dim_t Nd, dim_t EstNp>
using small_polyline = polyline<Nd, small_vector<point<Nd>, EstNp>>;

/// Fixed polyline.
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam Nv Number of vertices.
template <dim_t Nd, dim_t Nv>
using fixed_polyline = polyline<Nd, array<point<Nd>, Nv>>;

/// Bounded polyline
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam MaxNv Maximum number of vertices that the polyline can contain.
template <dim_t Nd, dim_t MaxNv>
using bounded_polyline = polyline<Nd, inline_vector<point<Nd>, MaxNv>>;

}  // namespace polyline_primitive
using polyline_primitive::bounded_polyline;
using polyline_primitive::small_polyline;
using polyline_primitive::fixed_polyline;

namespace simplex_primitive {

namespace simplex_detail {
struct ERROR_UNKNOWN_DIMENSION;
}  // namespace simplex_detail

// clang-format off

/// Simplex: points (1D), segments (2D), triangles(3D)
template <dim_t Nd>  
using simplex = std::conditional_t<Nd == 1, point<Nd>,
                std::conditional_t<Nd == 2, segment<Nd>,
                std::conditional_t<Nd == 3, triangle<Nd>,
                                   simplex_detail::ERROR_UNKNOWN_DIMENSION
                >>>;

// clang-format on

}  // namespace simplex_primitive

using simplex_primitive::simplex;

namespace any_primitive {
template <dim_t Nd>
struct any;
}
using any_primitive::any;

}  // namespace hm3::geometry
