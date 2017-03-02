#pragma once
/// \file
///
/// Bounding Volume Hierarchy node type
#include <hm3/geometry/data_structure/bvh/types.hpp>
#include <hm3/geometry/primitive/aabb.hpp>

namespace hm3::geometry::bvh {

/// BVH Node
template <dim_t Ad>
struct node {
  node_idx left       = node_idx{};
  node_idx right      = node_idx{};
  simplex_idx simplex = simplex_idx{};
  aabb<Ad> bound;

#ifdef HM3_DBG_BVH_VTK
  unsigned level = 0;
#endif

  node() = default;
  node(simplex_idx idx, aabb<Ad> box) : simplex(idx), bound(box) {}
  node(node_idx l, node_idx r, aabb<Ad> box) : left(l), right(r), bound(box) {}
  node(aabb<Ad> box) : bound(box) {}

  bool valid() const noexcept {
    return (simplex and !left and !right) or (!simplex and (left or right));
  }
};

}  // namespace hm3::geometry::bvh
