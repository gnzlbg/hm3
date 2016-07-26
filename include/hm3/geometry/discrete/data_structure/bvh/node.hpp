#pragma once
/// \file
///
/// Bounding Volume Hierarchy node type
#include <hm3/geometry/discrete/data_structure/bvh/types.hpp>
#include <hm3/geometry/discrete/primitive/aabb.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace bvh {

/// BVH Node
template <dim_t Nd>  //
struct node {
  node_idx left       = node_idx{};
  node_idx right      = node_idx{};
  simplex_idx simplex = simplex_idx{};
  aabb<Nd> bound;

#ifdef HM3_DBG_BVH_VTK
  unsigned level = 0;
#endif

  node() = default;
  node(simplex_idx idx, aabb<Nd> box) : simplex(idx), bound(box) {}
  node(node_idx l, node_idx r, aabb<Nd> box) : left(l), right(r), bound(box) {}
  node(aabb<Nd> box) : bound(box) {}
};

}  // namespace discrete
}  // namespace bvh
}  // namespace geometry
}  // namespace hm3
