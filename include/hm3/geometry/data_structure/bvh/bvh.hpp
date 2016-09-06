#pragma once
/// \file
///
/// Bounding Volume Hierarchy
#define HM3_DBG_BVH_VTK

#include <hm3/geometry/algorithm/bounding_length.hpp>
#include <hm3/geometry/algorithm/bounding_volume.hpp>
#include <hm3/geometry/algorithm/intersection/aabb_aabb.hpp>
#include <hm3/geometry/data_structure/bvh/node.hpp>
#include <hm3/geometry/data_structure/bvh/types.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/utility/timers.hpp>
#include <hm3/utility/vector.hpp>

//#define HM3_DBG_BVH  // enable for debugging output
#ifdef HM3_DBG_BVH
#define HM3_BVH_DBG_PRINT(...) fmt::print(__VA_ARGS__)
#else
#define HM3_BVH_DBG_PRINT(...)
#endif

namespace hm3::geometry {

/// Bounding-Volume Hierarchy.
namespace bvh {

/// Bonding-Voulme Hierarchy.
///
/// Stores a vector of nodes.
template <dim_t Nd>
struct bvh : dimensional<Nd> {
  using self = bvh<Nd>;

  using node_t         = node<Nd>;
  using node_storage_t = vector<node_t>;

  node_storage_t nodes_;
  HM3_TIME_TYPE(bvh);

  bvh()           = default;
  bvh(bvh const&) = default;
  bvh(bvh&&)      = default;
  bvh& operator=(bvh const&) = default;
  bvh& operator=(bvh&&) = default;

  /// Ids of all nodes in the BVH.
  auto node_ids() const noexcept {
    return view::ints(uint_t{0}, uint_t{nodes_.size()});
  }

  /// Node \p i
  auto const& node(node_idx i) const noexcept { return nodes_[*i]; }
  auto& node(node_idx i) noexcept { return nodes_[*i]; }

  /// Axis-Aligned Bounding Box of node \p i.
  auto bounding_box(node_idx i) const noexcept { return node(i).bound; }

  /// Simplex idx of node \p i
  auto simplex(node_idx i) const noexcept { return node(i).simplex; }

  /// Estimates the number of nodes from the number of triangles in the grid
  uint_t capacity_estimate(uint_t mesh_size) {
    // TODO: check this
    // - My intuition expect it to be ~O(N * log2N) but it turns out that for
    // real meshes this is not the case. It also depends on the split heuristic.
    // return mesh_size * std::log(mesh_size) + 1;
    return mesh_size * 2.5;
  }

  /// Non-reallocating push_back.
  node_idx push(node_t n) {
    HM3_ASSERT(nodes_.size() < nodes_.capacity(), "push back will reallocate!");
    node_idx r = node_idx{nodes_.size()};
    nodes_.push_back(n);
    return r;
  }

  bool is_leaf(node_idx i) const noexcept { return simplex(i) ? true : false; }

  /// Creates nodes for the simplices. These are the leafs of the BVH.
  template <typename Simplices>
  void create_simplex_nodes(Simplices const& mesh) {
    HM3_TIME("bvh", "rebuild", "create_simplex_nodes");
    nodes_.reserve(mesh.size());
    for (auto&& i : mesh.face_ids()) {
      node_t n(i, bounding_volume.aabb(mesh.simplex(i)));
      push(n);
    }
  }

  /// Sort the node range in range [\p b, \p e) in increasing centroid component
  /// \p d
  void sort_nodes_across_centroid(node_idx b, node_idx e, dim_t d) {
    HM3_TIME("bvh", "rebuild", "create_tree_nodes", "split",
             "sort_nodes_across_centroid");
    auto b_ = begin(nodes_) + (*b);
    auto e_ = begin(nodes_) + (*e);
    std::sort(b_, e_, [d](auto&& na, auto&& nb) {
      auto xa = centroid(na.bound);
      auto xb = centroid(nb.bound);
      return xa(d) < xb(d);
    });
  }

  /// Bounding box of all the nodes in range [\p b, \p e).
  aabb<Nd> bounding_box(node_idx b, node_idx e) const {
    return bounding_volume.aabb(
     ranges::make_iterator_range(begin(nodes_) + (*b), begin(nodes_) + (*e))
     | view::transform([](auto&& n) { return n.bound; }));
  }

  /// Returns the surface area of the bounding box that encloses all nodes in
  /// range [\p b, \p e).
  num_t surface_area_range(node_idx b, node_idx e) const {
    HM3_TIME("bvh", "rebuild", "create_tree_nodes", "split",
             "surface_area_range");
    return surface_area(bounding_box(b, e));
  };

  /// Split the child nodes of \p p in range [\p b, \p e)
  /// Returns the id of the node that provides the best split.
  ///
  /// The nodes are sorted in increasing centroid component along the split
  /// dimension.
  node_idx split(node_t& p, node_idx b, node_idx e) {
    HM3_TIME("bvh", "rebuild", "create_tree_nodes", "split");
    // Find the longest direction of the AABB
    // => to split along it.
    dim_t split_d = [&]() {
      auto max_l = bounding_length.max(p.bound);
      auto ls    = bounding_length.all(p.bound);
      for (dim_t d = 0; d < Nd; ++d) {
        if (!(ls(d) < max_l)) { return d; }
      }
      HM3_FATAL_ERROR("!");
    }();

    sort_nodes_across_centroid(b, e, split_d);

    // Surface Area Heuristic (SAH): perform the split such that S_C / S_P * N_C
    // is similar for both sides of the split. Here:
    // - S_C is the surface of the bounding box of the child node (which we need
    //   to compute for each possible split),
    // - S_P is the surface of the bounding box of the parent, and
    // - N_C is the number of triangles of the child node.
    num_t sp   = surface_area(p.bound);
    num_t cost = math::highest<num_t>;
    node_idx result{};
    auto start = b + 1;
    auto end   = e;
    HM3_ASSERT(start < end, "");
    // Use binning across 8 planes: since the cost function will be in general
    // monotonic (because the triangles are sorted), the stride here is used to
    // compute the SAH on at most 8 planes:
    auto no_planes  = end - start;
    node_idx stride = no_planes <= 8 ? 1 : no_planes / 8;
    for (auto split_idx = start; split_idx < end; split_idx += stride) {
      num_t nl    = *split_idx - *b;
      num_t nr    = *e - *split_idx;
      num_t sl    = surface_area_range(b, split_idx);
      num_t sr    = surface_area_range(split_idx, e);
      num_t cost_ = sl / sp * nl + sr / sp * nr;

      if (cost_ < cost) {
        cost   = cost_;
        result = split_idx;
      }

      // range is sorted => break if the cost starts increasing (chances are we
      // found a good enough minimum):
      if (cost_ > cost and result) { break; }
    }
    HM3_ASSERT(result, "did not found a viable split?!!");
    HM3_ASSERT(result - b > 0, "");
    HM3_ASSERT(e - result > 0, "");
    return result;
  };

  /// Insert child nodes of node \p n containing nondes in range [\p b, \p e).
  void insert_tree_nodes(node_t& n, node_idx b, node_idx e) {
    HM3_BVH_DBG_PRINT("itn: parent {}, range [{}, {}), #={}\n", &n - &nodes_[0],
                      b, e, e - b);
    HM3_ASSERT(e - b > node_idx{1}, "[{}, {})", b, e);

    // If the #of overlapped nodes is 2 we are done:
    if (e - b == node_idx{2}) {
      n.left  = b;
      n.right = b + node_idx{1};
      return;
    }

    node_idx split_node = split(n, b, e);

    auto no_nodes_left  = split_node - b;
    auto no_nodes_right = e - split_node;
    HM3_BVH_DBG_PRINT("  split: {}, left: [{}, {}) #{}, right: [{}, {}) #{}\n",
                      split_node, b, split_node, no_nodes_left, split_node, e,
                      no_nodes_right);

    if (no_nodes_left == node_idx{1}) {
      n.left = b;
    } else {
      auto left_bbox = bounding_box(b, split_node);
      node_t left(left_bbox);
      n.left = push(left);
      insert_tree_nodes(nodes_.back(), b, split_node);
    }

    if (no_nodes_right == node_idx{1}) {
      n.right = split_node;
    } else {
      auto right_bbox = bounding_box(split_node, e);
      node_t right(right_bbox);
      n.right = push(right);
      insert_tree_nodes(nodes_.back(), split_node, e);
    }
    HM3_BVH_DBG_PRINT("  n: {}, L: {}, R: {}\n", &n - &nodes_[0], n.left,
                      n.right);
  }

  /// Create non-leaf tree nodes.
  ///
  /// \pre The leaf nodes must already be inserted in the tree.
  template <typename Simplices>
  node_idx create_tree_nodes(Simplices const& mesh) {
    HM3_TIME("bvh", "rebuild", "create_tree_nodes");
    HM3_ASSERT(nodes_.size() == mesh.size(), "");
    HM3_ASSERT(nodes_.capacity() == capacity_estimate(mesh.size()), "");

    // Create the root node:
    node_t root;
    root.bound = bounding_volume.aabb(
     nodes_ | view::transform([](auto&& n) { return n.bound; }));

    HM3_ASSERT(nodes_.size() < nodes_.capacity(), "");
    node_idx root_node_idx = node_idx{nodes_.size()};
    push(root);

    {  // insert tree nodes starting at the root
      node_idx b = node_idx{0};
      node_idx e = node_idx{nodes_.size() - 1};
      insert_tree_nodes(nodes_.back(), b, e);
    }
    return root_node_idx;
  }

#ifdef HM3_DBG_BVH_VTK
  /// Sets the level of all nodes in the tree (debug only)
  void set_level(node_idx current, int current_level) {
    nodes_[*current].level = current_level;
    if (nodes_[*current].left) {
      set_level(nodes_[*current].left, current_level + 1);
    }
    if (nodes_[*current].right) {
      set_level(nodes_[*current].right, current_level + 1);
    }
  }
#endif

  /// Move node \p current to its correct position \p should and updates the
  /// simplex indices.
  node_idx update_correct_node_positions(vector<node_idx>& pos,
                                         node_idx current, node_idx should) {
    auto& n = nodes_[*current];
    HM3_BVH_DBG_PRINT("c: {} (L: {}, R: {}), s: {}\n", current, n.left, n.right,
                      should);
    pos[*current] = *should;
    ++should;

    if (n.left) {
      HM3_BVH_DBG_PRINT(" => L\n");
      should = update_correct_node_positions(pos, n.left, should);
    }
    if (n.right) {
      HM3_BVH_DBG_PRINT(" => R\n");
      should = update_correct_node_positions(pos, n.right, should);
    }
    HM3_BVH_DBG_PRINT(" => T\n");
    return should;
  }

  /// Sorts the nodes in depth-first order starting ad the \p root node of the
  /// bvh.
  void sort_nodes(node_idx root_node) {
    HM3_TIME("bvh", "rebuild", "sort_nodes");
    vector<node_idx> correct_positions(nodes_.size());

    auto last
     = update_correct_node_positions(correct_positions, root_node, node_idx{0});
    HM3_ASSERT(*last == nodes_.size(), "{} == {}", *last, nodes_.size());

    // correct all tree node positions
    for (auto i : node_ids()) {
      auto&& n = node(i);
      n.left   = n.left ? correct_positions[*n.left] : n.left;
      n.right  = n.right ? correct_positions[*n.right] : n.right;
    }

    // resort
    for (auto i : node_ids()) {
      while (i != *correct_positions[i]) {
        auto next = correct_positions[i];
        ranges::swap(nodes_[i], nodes_[*next]);
        ranges::swap(correct_positions[i], correct_positions[*next]);
      }
    }

    // check that it is sorted:
    HM3_ASSERT(
     [&]() {
       for (auto i : node_ids()) {
         HM3_ASSERT(correct_positions[i] == i, "correct {} == {} (is)",
                    correct_positions[i], i);
       }
       return true;
     }(),
     "not sorted!");
  }

  /// Rebuilds the BVH
  ///
  /// The root node is the first node in the tree.
  template <typename Simplices>
  void rebuild(Simplices const& mesh) {
    HM3_TIME("bvh", "rebuild");
    HM3_ASSERT(mesh.size() > 0_u, "trying to create a BVH from an empty mesh");
    nodes_.clear();
    uint_t size_ = mesh.size();

    fmt::print("mesh size: {} => reserved capacity: {}\n", size_,
               capacity_estimate(size_));

    nodes_.reserve(capacity_estimate(size_));

    create_simplex_nodes(mesh);

    node_idx root_node = create_tree_nodes(mesh);

    sort_nodes(root_node);

    fmt::print("Mesh size: {}, BVH #nodes: {}, #capacity: {}\n", size_,
               nodes_.size(), nodes_.capacity());
    nodes_.shrink_to_fit();

#ifdef HM3_DBG_BVH_VTK
    {
      HM3_TIME("bvh", "rebuild", "set_level");
      set_level(node_idx{0}, 0);
    }
#endif
  }

  /// Adds indices of the simplices whose bounding box is intersected by \p
  /// target to the vector \p v for all the simplices overlapped by the bounding
  /// box of the node \p n.
  template <typename Vector>
  void add_intersected_nodes(node_idx n, aabb<Nd> const& target,
                             Vector& v) const {
    // If the bounding box of \p n does not intersect \p target we are done (the
    // bounding box of the children won't intersect target).
    if (!intersection.test(bounding_box(n), target)) { return; }

    // The bounding box of \p n intersects \p target and corresponds to that of
    // a simplex, so we add the simplex idx to the vector and are done
    if (is_leaf(n)) {
      v.push_back(simplex(n));
      return;
    }

    // The bounding box of \p n intersects \p target but it is not a leaf bbox,
    // so we recurse down to its two children, if present:
    if (auto l = node(n).left) { add_intersected_nodes(l, target, v); }
    if (auto r = node(n).right) { add_intersected_nodes(r, target, v); }
  }

  /// Appends to the vector \p result the ids of the simplices whose bounding
  /// boxes intersect with aabb \p a.
  template <typename Vector>
  void aabb_intersection(aabb<Nd> const& target, Vector& result) const {
    auto root_node = node_idx{0};
    add_intersected_nodes(root_node, target, result);
  }

  /// Returns a vector of simplex ids whose bounding boxes intersect with aabb
  /// \p a.
  small_vector<simplex_idx, 128> aabb_intersection(
   aabb<Nd> const& target) const {
    small_vector<simplex_idx, 128> result;
    aabb_intersection(target, result);
    return result;
  }

  /// An AABB \p target intersects the bounding box of at least one simplex if
  /// it intersects the bounding box of the root node of the BVH.
  bool aabb_intersection_test(aabb<Nd> const& target) const {
    return intersection.test(target, bounding_box(node_idx{0}));
  }
};

}  // namespace bvh
}  // namespace hm3::geometry
