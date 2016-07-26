#pragma once
/// \file
///
/// Bounding Volume Hierarchy
#include <hm3/geometry/bvh/bvh.hpp>
#include <hm3/geometry/bvh/node.hpp>
#include <hm3/geometry/bvh/types.hpp>

namespace hm3 {
namespace geometry {

template <dim_t Nd>  //
using bounding_volume_hierarchy = bvh::bvh<Nd>;

}  // namespace geometry
}  // namespace hm3
