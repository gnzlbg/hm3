#pragma once
/// \file
///
/// Box: AABB of constant length
#include <hm3/geometry/discrete/primitive/box/bounding_length.hpp>
#include <hm3/geometry/discrete/primitive/box/bounding_volume.hpp>
#include <hm3/geometry/discrete/primitive/box/box.hpp>
#include <hm3/geometry/discrete/primitive/box/centroid.hpp>
#include <hm3/geometry/discrete/primitive/box/contains.hpp>
#include <hm3/geometry/discrete/primitive/box/direction.hpp>
#include <hm3/geometry/discrete/primitive/box/integral.hpp>
#include <hm3/geometry/discrete/primitive/box/ostream.hpp>
#include <hm3/geometry/discrete/primitive/box/perimeter.hpp>
#include <hm3/geometry/discrete/primitive/box/vertices.hpp>
// #include <hm3/geometry/discrete/primitive/box/polygon.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

/// Box primitive
namespace box_primitive {}

}  // namespace discrete

using discrete::box;

}  // namespace geometry
}  // namespace hm
