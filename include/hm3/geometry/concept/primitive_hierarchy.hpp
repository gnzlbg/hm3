#pragma once
/// \file
///
/// Trait hierarchy of geometry objects.
#include <hm3/geometry/concept/associated_dimension.hpp>

namespace hm3::geometry {

namespace trait {

/// \name Primitive Hierarchy
///@{

template <dim_t Ad>
struct point : object<Ad, 0> {};
template <dim_t Ad>
struct vector : object<Ad, 1> {};
template <dim_t Ad>
struct line : object<Ad, 1> {};
template <dim_t Ad>
struct segment : object<Ad, 1> {};
template <dim_t Ad>
struct ray : object<Ad, 1> {};
template <dim_t Ad>
struct polyline : object<Ad, 1> {};
template <dim_t Ad /*, Ad > 1 */>
struct polygon : object<Ad, 2> {};
struct polysurface : object<3, 2> {};
struct polyhedron : object<3, 3> {};

template <dim_t Ad>
struct plane : object<Ad, Ad - 1> {};
template <>
struct plane<2> : line<2> {};

///@} // Primitive Hierarchy

template <dim_t Ad>
struct aabb;
template <>
struct aabb<1> : segment<1> {};
template <>
struct aabb<2> : polygon<2> {};
template <>
struct aabb<3> : polyhedron {};

template <dim_t Ad>
struct box : aabb<Ad> {};

template <dim_t Ad>
struct triangle : polygon<Ad> {};

}  // namespace trait

}  // namespace hm3::geometry
