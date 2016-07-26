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
#include <hm3/geometry/discrete/access.hpp>
#include <hm3/geometry/discrete/algorithm/bounding_length.hpp>
#include <hm3/geometry/discrete/algorithm/bounding_volume.hpp>
#include <hm3/geometry/discrete/algorithm/centroid.hpp>
#include <hm3/geometry/discrete/algorithm/integral.hpp>
#include <hm3/geometry/discrete/algorithm/intersection.hpp>
#include <hm3/geometry/discrete/algorithm/normal.hpp>
#include <hm3/geometry/discrete/primitive/point.hpp>
#include <hm3/geometry/discrete/primitive/vector.hpp>
#include <hm3/geometry/rank.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace concepts {

namespace rc = ranges::concepts;

template <typename T> struct is_bounded : std::true_type {};

template <typename T> struct is_primitive : std::true_type {};

struct primitive : rc::refines<rc::Regular, geometry::concepts::Ranked,
                               access::concepts::Vertex> {
  template <typename T, typename UT = uncvref_t<T>>
  static auto requires_(T&& t) -> decltype(           //
   rc::valid_expr(                                    //
    rc::is_true(is_bounded<UT>{}),                    //
    rc::is_true(is_primitive<UT>{}),                  //
    static_cast<point<UT::dimension()>>(centroid(t))  //
    ));
};

using Primitive = primitive;

template <typename T> struct is_segment_primitive : std::true_type {};

// SegmentPrimitive
struct segment_primitive : rc::refines<Primitive> {
  template <typename T, typename UT = uncvref_t<T>>
  static auto requires_(T&& t) -> decltype(             //
   rc::valid_expr(                                      //
    rc::is_true(is_segment_primitive<UT>{}),            //
    static_cast<num_t>(length(t)),                      //
    static_cast<vector<UT::dimension()>>(direction(t))  //
    ));
};

using SegmentPrimitive = segment_primitive;

template <typename T> struct is_surface_primitive : std::true_type {};

// SurfacePrimitive
struct surface_primitive : rc::refines<Primitive> {
  template <typename T, typename UT = uncvref_t<T>>
  static auto requires_(T&& t) -> decltype(          //
   rc::valid_expr(                                   //
    rc::is_true(is_surface_primitive<UT>{}),         //
    static_cast<num_t>(area(t)),                     //
    static_cast<num_t>(perimeter(t)),                //
    static_cast<vector<UT::dimension()>>(normal(t))  //
    ));
};

using SurfacePrimitive = surface_primitive;

template <typename T> struct is_volume_primitive : std::true_type {};

// VolumePrimitive
struct volume_primitive : rc::refines<Primitive> {
  template <typename T, typename UT = uncvref_t<T>>
  static auto requires_(T&& t) -> decltype(  //
   rc::valid_expr(                           //
    rc::is_true(is_volume_primitive<UT>{}),  //
    static_cast<num_t>(volume(t)),           //
    static_cast<num_t>(surface_area(t)),     //
    static_cast<num_t>(perimeter(t))         //
    ));
};

using VolumePrimitive = volume_primitive;

}  // namespace concepts

template <typename T>
using Primitive = concepts::rc::models<concepts::Primitive, uncvref_t<T>>;

template <typename T>
using SegmentPrimitive
 = concepts::rc::models<concepts::SegmentPrimitive, uncvref_t<T>>;

template <typename T>
using SurfacePrimitive
 = concepts::rc::models<concepts::SurfacePrimitive, uncvref_t<T>>;

template <typename T>
using VolumePrimitive
 = concepts::rc::models<concepts::VolumePrimitive, uncvref_t<T>>;

}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
