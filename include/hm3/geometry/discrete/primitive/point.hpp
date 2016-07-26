#pragma once
/// \file
///
/// Point type.
#include <hm3/geometry/discrete/concepts.hpp>
#include <hm3/geometry/discrete/primitive/point/centroid.hpp>
#include <hm3/geometry/discrete/primitive/point/normal.hpp>
#include <hm3/geometry/discrete/primitive/point/point.hpp>
#include <hm3/geometry/discrete/primitive/point/vertices.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

/// Point vertex primitive
namespace point_primitive {}

}  // namespace discrete

using discrete::point;

}  // namespace geometry
}  // namespace hm3
