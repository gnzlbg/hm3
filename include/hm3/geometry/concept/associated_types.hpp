#pragma once
/// \file
///
/// Associated geometric types
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/geometry/concept/associated_dimension.hpp>
#include <hm3/geometry/primitive/fwd.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace associated {

template <typename T>
struct num_type {
  using type = num_t;
};

template <typename T>
using num_type_t = typename num_type<uncvref_t<T>>::type;

/// \name Index types
///@{

namespace customization {

template <typename T>
struct vertex_index {
  using type = dim_t;
};

}  // namespace customization

template <typename T>
using vertex_index_t = typename customization::vertex_index<T>::type;

namespace customization {

template <typename T>
struct edge_index {
  using type = dim_t;
};

}  // namespace customization

template <typename T>
using edge_index_t = typename customization::edge_index<T>::type;

namespace customization {

template <typename T>
struct face_index {
  using type = dim_t;
};

}  // namespace customization

template <typename T>
using face_index_t = typename customization::face_index<T>::type;

///@} // Index types

/// \name Primitive types
///@{

namespace customization {

template <typename T, typename = void>
struct point {
  using type = hm3::geometry::point<ad_v<uncvref_t<T>>>;
};

template <typename T>
struct point<T, std::void_t<typename uncvref_t<T>::point_value_type>> {
  using type = typename uncvref_t<T>::point_value_type;
};

}  // namespace customization

template <typename T>
using point_t = typename customization::point<T>::type;

namespace customization {

template <typename T, typename = void>
struct vector {
  using type = hm3::geometry::vec<ad_v<uncvref_t<T>>>;
};

template <typename T>
struct vector<T, std::void_t<typename uncvref_t<T>::vector_value_type>> {
  using type = typename uncvref_t<T>::vector_value_type;
};

}  // namespace customization

template <typename T>
using vector_t = typename customization::vector<T>::type;

namespace customization {

template <typename T, typename = void>
struct aabb {
  using type = hm3::geometry::aabb<ad_v<uncvref_t<T>>>;
};

template <typename T>
struct aabb<T, std::void_t<typename uncvref_t<T>::aabb_value_type>> {
  using type = typename uncvref_t<T>::aabb_value_type;
};

}  // namespace customization

template <typename T>
using aabb_t = typename customization::aabb<T>::type;

namespace customization {

template <typename T, typename = void>
struct box {
  using type = hm3::geometry::box<ad_v<uncvref_t<T>>>;
};

template <typename T>
struct box<T, std::void_t<typename uncvref_t<T>::box_value_type>> {
  using type = typename uncvref_t<T>::box_value_type;
};

}  // namespace customization

template <typename T>
using box_t = typename customization::box<T>::type;

namespace customization {

template <typename T, typename = void>
struct line {
  using type = hm3::geometry::line<ad_v<uncvref_t<T>>>;
};

template <typename T>
struct line<T, std::void_t<typename uncvref_t<T>::line_value_type>> {
  using type = typename uncvref_t<T>::line_type;
};

}  // namespace customization

template <typename T>
using line_t = typename customization::line<T>::type;

namespace customization {

template <typename T, typename = void>
struct ray {
  using type = hm3::geometry::ray<ad_v<uncvref_t<T>>>;
};

template <typename T>
struct ray<T, std::void_t<typename uncvref_t<T>::ray_value_type>> {
  using type = typename uncvref_t<T>::ray_type;
};

}  // namespace customization

template <typename T>
using ray_t = typename customization::ray<T>::type;

namespace customization {

template <typename T, typename = void>
struct polyline {
  using type = hm3::geometry::polyline<ad_v<uncvref_t<T>>>;
};

template <typename T>
struct polyline<T, std::void_t<typename uncvref_t<T>::polyline_value_type>> {
  using type = typename uncvref_t<T>::polyline_value_type;
};

}  // namespace customization

template <typename T>
using polyline_t = typename customization::polyline<T>::type;

namespace customization {

template <typename T, dim_t Nv, typename = void>
struct fixed_polygon {
  using type = hm3::geometry::fixed_polygon<ad_v<uncvref_t<T>>, Nv>;
};

}  // namespace customization

template <typename T, dim_t Nv>
using fixed_polygon_t = typename customization::fixed_polygon<T, Nv>::type;

namespace customization {

template <typename T, typename = void>
struct edge {
  using type = hm3::geometry::segment<ad_v<uncvref_t<T>>>;
};

template <typename T>
struct edge<T, std::void_t<typename uncvref_t<T>::edge_value_type>> {
  using type = typename uncvref_t<T>::edge_value_type;
};

}  // namespace customization

template <typename T>
using edge_t = typename customization::edge<T>::type;

namespace customization {

template <typename T, typename = void>
struct face {
  using type = hm3::geometry::polygon<ad_v<uncvref_t<T>>>;
};

template <typename T>
struct face<T, std::void_t<typename uncvref_t<T>::face_value_type>> {
  using type = typename uncvref_t<T>::face_value_type;
};

}  // namespace customization

template <typename T>
using face_t = typename customization::face<T>::type;

///@}

}  // namespace associated

}  // namespace hm3::geometry
