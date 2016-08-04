#pragma once
/// \file
///
/// Primitive forward declarations.
#include <hm3/types.hpp>

namespace hm3::geometry {

namespace point_primitive {
template <dim_t Nd> struct point;
}  // namespace point_primitive
using point_primitive::point;

namespace vec_primitive {
template <dim_t Nd> struct vec;
}  // namespace vec_primitive
using vec_primitive::vec;

namespace aabb_primitive {
template <dim_t Nd> struct aabb;
}  // namespace aabb_primitive
using aabb_primitive::aabb;

namespace box_primitive {
template <dim_t Nd> struct box;
}  // namespace box_primitive
using box_primitive::box;

namespace segment_primitive {
template <dim_t Nd> struct segment;
}  // namespace segment_primitive
using segment_primitive::segment;

namespace polygon_primitive {
template <dim_t Nd, dim_t Np> struct fixed_polygon;
template <dim_t Nd, dim_t MaxNp> struct bounded_polygon;
}  // namespace polygon_primitive
using polygon_primitive::fixed_polygon;
using polygon_primitive::bounded_polygon;
template <dim_t Nd> using triangle = fixed_polygon<Nd, 3>;
template <dim_t Nd> using quad     = fixed_polygon<Nd, 4>;

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

}  // namespace hm3::geometry
