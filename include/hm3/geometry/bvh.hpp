#pragma once
/// \file
///
/// Bounding Volume Hierarchy
#include <hm3/geometry/aabb.hpp>
#include <hm3/utility/variant.hpp>
#include <vector>

namespace hm3 {
namespace geometry {

/// Bounding-Volume Hierarchy
namespace bvh {

/// BVH Node index (weakly-typed)
using idx_t = suint_t;

/// Index of a BVH node within a BVH
using node_idx
 = compact_optional<empty_scalar_value<idx_t,
                                       std::numeric_limits<idx_t>::max()>,
                    struct node_idx_tag>;

/// BVH Node
template <dim_t Nd>  //
struct node {
  node_idx left       = node_idx{};
  node_idx right      = node_idx{};
  simplex_idx simplex = simplex_idx{};
  aabb<Nd> bound;

  node() = default;
  node(simplex_idx idx, aabb<Nd> box) : simplex(idx), bound(box) {}
  node(node_idx l, node_idx r, aabb<Nd> box) : left(l), right(r), bound(box) {}
};

/// BVH
template <dim_t Nd>  //
struct tree {
  using node_t         = node<Nd>;
  using node_storage_t = std::vector<node_t>;
  node_storage_t nodes;
  node_idx root;

  bvh()           = default;
  bvh(bvh const&) = default;
  bvh(bvh&&)      = default;
  bvh& operator=(bvh const&) = default;
  bvh& operator=(bvh&&) = default;

  template <typename SimplexRange>  //
  node_idx insert(SimplexRange& all, SimplexRange& current) {
    node n;
    // only one triangle:
    if (size(current) == 1_su) {
      auto triangle = *begin(current);
      auto tidx     = begin(current) - begin(all);
      n             = node(tidx, axis_aligned_bounding_box(triangle));
    } else {
      // more than one triangle:
      auto bb   = compute_bounding_box(current);
      auto axis = compute_splitting_axis(current);

      auto f  = begin(current);
      auto l  = end(current);
      auto pp = stable_parition(
       f, l, [axis](auto&& a) { return is_left_of_axis(a, axis); });

      auto splitted = split(current, pp);

      n = node(insert(all, current), insert(all, current), simplex_idx{}, bb);
    }
    node_idx idx(nodes.size());
    nodes.push_back(std::move(n));
    return idx;
  }

  template <typename SimplexRange>  //
  void rebuild(SimplexRange& rng) {
    nodes.clear();
    auto size_             = size(rng);
    auto capacity_estimate = size_ * std::log(size_);
    nodes.reserve(capacity_estimate);
    fmt::print("reserved capacity: {}\n", capacity_estimate);
    insert(rng, rng);
  }
};

}  // namespace bvh
}  // namespace geometry
}  // namespace hm3
