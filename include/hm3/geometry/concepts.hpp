#pragma once
/// \file
///
/// Vertex Primitive concepts
///
/// The hierarchy is as follows:
///
/// VertexPrimitive (access/iteration over vertices)
///  |---> SegmentPrimitive (line-like => length)
///  |---> AreaPrimitive (surface-like => area, normal, perimeter)
///  |---> VolumePrimitive (volume-like => volume, surface-area)
///
#include <hm3/geometry/access.hpp>
#include <hm3/geometry/algorithm/centroid.hpp>
#include <hm3/geometry/algorithm/direction.hpp>
#include <hm3/geometry/algorithm/integral.hpp>
#include <hm3/geometry/algorithm/normal.hpp>
#include <hm3/geometry/primitive/fwd.hpp>
#include <hm3/geometry/primitive/point/point.hpp>
#include <hm3/geometry/primitive/vec.hpp>
#include <hm3/geometry/rank.hpp>

namespace hm3::geometry {

/// Geometry concepts.
namespace concepts {

namespace rc = ranges::concepts;

template <typename T> struct is_bounded : std::true_type {};

template <typename T> struct is_primitive : std::true_type {};

struct primitive : rc::refines<rc::Regular, geometry::concepts::Ranked,
                               access::concepts::Vertex> {
  template <typename T, typename UT = uncvref_t<T>>
  static auto requires_(T&& t) -> decltype(  //
   rc::valid_expr(                           //
    rc::is_true(is_bounded<UT>{}),           //
    rc::is_true(is_primitive<UT>{})          //
    ));
};

using Primitive = primitive;

// SegmentPrimitive (Rank 1)
struct segment_primitive : rc::refines<Primitive> {
  template <typename T, typename UT = uncvref_t<T>>
  static auto requires_(T&& t) -> decltype(  //
   rc::valid_expr(                           //
    rc::is_true(Ranked<T, UT::dimension(), 1>{})));
};

using Segment = segment_primitive;

// SurfacePrimitive (Rank: Nd - 1)
struct surface_primitive : rc::refines<Primitive> {
  template <typename T, typename UT = uncvref_t<T>>
  static auto requires_(T&& t) -> decltype(  //
   rc::valid_expr(                           //
    rc::is_true(Ranked<T, UT::dimension(), UT::dimension() - 1>{})));
};

using Surface = surface_primitive;

// VolumePrimitive (Rank Nd)
struct volume_primitive : rc::refines<Primitive> {
  template <typename T, typename UT = uncvref_t<T>>
  static auto requires_(T&& t) -> decltype(  //
   rc::valid_expr(                           //
    rc::is_true(Ranked<T, UT::dimension(), UT::dimension()>{})));
};

using Volume = volume_primitive;

template <typename T> struct is_polygon : std::true_type {};

struct polygon : rc::refines<Primitive> {
  template <typename T>
  static auto requires_(T&& t) -> decltype(       //
   rc::valid_expr(                                //
    rc::is_true(is_polygon<rc::uncvref_t<T>>{}),  //
    rc::is_true(Ranked<T, dimension(T{}), 2>{})   //
    ));
};

using Polygon = polygon;

}  // namespace concepts

template <typename T>
using Primitive = concepts::rc::models<concepts::Primitive, uncvref_t<T>>;

template <typename T>
using Segment = concepts::rc::models<concepts::Segment, uncvref_t<T>>;

template <typename T>
using Surface = concepts::rc::models<concepts::Surface, uncvref_t<T>>;

template <typename T>
using Volume = concepts::rc::models<concepts::Volume, uncvref_t<T>>;

template <typename T, dim_t Nd = math::highest<dim_t>>
using Polygon
 = meta::and_<concepts::rc::models<concepts::Polygon, ranges::uncvref_t<T>>,
              meta::bool_<Dimensional<T, Nd>{}>>;

}  // namespace hm3::geometry
