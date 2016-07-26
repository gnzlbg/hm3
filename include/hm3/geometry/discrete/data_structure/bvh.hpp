#pragma once
/// \file
///
/// Bounding Volume Hierarchy
#include <hm3/geometry/discrete/data_structure/bvh/bvh.hpp>
#include <hm3/geometry/discrete/data_structure/bvh/node.hpp>
#include <hm3/geometry/discrete/data_structure/bvh/types.hpp>

namespace hm3 {
namespace geometry {

namespace discrete {

template <dim_t Nd>  //
using bounding_volume_hierarchy = bvh::bvh<Nd>;

}  // namespace discrete

using discrete::bounding_volume_hierarchy;

}  // namespace geometry
}  // namespace hm3
