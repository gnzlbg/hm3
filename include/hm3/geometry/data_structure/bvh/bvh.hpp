#pragma once
/// \file
///
/// Bounding Volume Hierarchy
#define HM3_DBG_BVH_VTK

#include <hm3/geometry/algorithm/bounding_length.hpp>
#include <hm3/geometry/algorithm/bounding_volume.hpp>
#include <hm3/geometry/algorithm/integral.hpp>
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/data_structure/bvh/node.hpp>
#include <hm3/geometry/data_structure/bvh/types.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/utility/timers.hpp>
#include <hm3/utility/vector.hpp>

//#define HM3_DBG_BVH  // enable for debugging output
#ifdef HM3_DBG_BVH
#define HM3_BVH_DBG_PRINT(...) ascii_fmt::out(__VA_ARGS__)
#else
#define HM3_BVH_DBG_PRINT(...)
#endif

namespace hm3::geometry {

/// Bounding-Volume Hierarchy.
namespace bvh {

/// Bonding-Volume Hierarchy.
///
/// Stores a hierarchy of axis-aligned bounding boxes. The leafs of the
/// hierarchy contain the index of a simplex inside some mesh.
template <dim_t Ad>
struct bvh : with_ambient_dimension<Ad> {
  using self = bvh<Ad>;

  using node_t         = node<Ad>;
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
  aabb<Ad> bounding_aabb(node_idx i) const noexcept { return node(i).bound; }

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

  /// Is a node a leaf node of the BVH?
  ///
  /// That is, does it contain a simplex?
  /// Nodes can either contain a simplex, or contain either a left or right
  /// node.
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
  aabb<Ad> bounding_aabb(node_idx b, node_idx e) const {
    HM3_TIME("bvh", "rebuild", "bounding_aabb");
    return bounding_volume.aabb(
     ranges::make_iterator_range(begin(nodes_) + (*b), begin(nodes_) + (*e))
     | view::transform([](auto&& n) { return n.bound; }));
  }

  template <typename T>
  static num_t surface_area(T&& t) {
    HM3_TIME("bvh", "rebuild", "surface_area");
    return integral.boundary(std::forward<T>(t));
  }

  /// Returns the surface area of the bounding box that encloses all nodes in
  /// range [\p b, \p e).
  num_t surface_area_range(node_idx b, node_idx e) const {
    return surface_area(bounding_aabb(b, e));
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
      for (dim_t d = 0; d < Ad; ++d) {
        if (!(ls(d) < max_l)) { return d; }
      }
      HM3_FATAL_ERROR("!");
    }();

    sort_nodes_across_centroid(b, e, split_d);

    {
      HM3_TIME("bvh", "rebuild", "create_tree_nodes", "split",
               "surface_area_heuristic");
      // Surface Area Heuristic (SAH): perform the split such that S_C / S_P *
      // N_C is similar for both sides of the split. Here: - S_C is the surface
      // of the bounding box of the child node (which we need
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
      // monotonic (because the triangles are sorted), the stride here is used
      // to compute the SAH on at most 8 planes:
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

        // range is sorted => break if the cost starts increasing (chances are
        // we found a good enough minimum):
        if (cost_ > cost and result) { break; }
      }
      HM3_ASSERT(result, "did not found a viable split?!!");
      HM3_ASSERT(result - b > 0, "");
      HM3_ASSERT(e - result > 0, "");
      return result;
    }
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
      auto left_bbox = bounding_aabb(b, split_node);
      node_t left(left_bbox);
      n.left = push(left);
      insert_tree_nodes(nodes_.back(), b, split_node);
    }

    if (no_nodes_right == node_idx{1}) {
      n.right = split_node;
    } else {
      auto right_bbox = bounding_aabb(split_node, e);
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

  /// Bounding aabb of the BVH
  auto bounding_aabb() const noexcept { return bounding_aabb(node_idx{0}); }

  /// Bounding box of the BVH
  auto bounding_box() const noexcept {
    return bounding_volume.box(bounding_aabb());
  }

  /// Are all nodes valid?
  bool valid_nodes() const noexcept {
    for (auto&& i : node_ids()) {
      if (not node(i).valid()) {
        HM3_ASSERT(false, "node with id {} is not valid!", i);
        return false;
      }
    }
    return true;
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

    // ascii_fmt::out("mesh size: {} => reserved capacity: {}\n", size_,
    //                capacity_estimate(size_));

    nodes_.reserve(capacity_estimate(size_));

    create_simplex_nodes(mesh);

    node_idx root_node = create_tree_nodes(mesh);

    sort_nodes(root_node);

    // ascii_fmt::out("Mesh size: {}, BVH #nodes: {}, #capacity: {}\n", size_,
    //                nodes_.size(), nodes_.capacity());
    nodes_.shrink_to_fit();

    HM3_ASSERT(valid_nodes(), "invalid nodes present");

#ifdef HM3_DBG_BVH_VTK
    {
      HM3_TIME("bvh", "rebuild", "set_level");
      set_level(node_idx{0}, 0);
    }
#endif
  }

  enum class visitor_action { visit_both, visit_left, visit_right, done };

  /// Visit each valid node of the BVH calling `f(node_idx) -> visitor_action`,
  /// where the visitor action controls what to do after calling f.
  template <typename F>
  void for_each(F&& f, node_idx current = node_idx{0}) const noexcept {
    // If there is no node we are done:
    if (not current) { return; }

    visitor_action action = f(current);
    switch (action) {
      case visitor_action::done: {
        break;
      }
      case visitor_action::visit_left: {
        for_each(f, node(current).left);
        break;
      }
      case visitor_action::visit_right: {
        for_each(f, node(current).right);
        break;
      }
      case visitor_action::visit_both: {
        for_each(f, node(current).left);
        for_each(f, node(current).right);
        break;
      }
      default: { HM3_FATAL_ERROR("unknown visitor action"); }
    }
  }

  struct node_dist {
    node_idx n{};
    num_t d = math::highest<num_t>;

    bool valid() const noexcept {
      return n != node_idx{} and not math::approx(d, math::highest<num_t>);
    }
  };

  /// Gets the node of the simplex closest to \p p.
  template <typename P, typename M, CONCEPT_REQUIRES_(Point<uncvref_t<P>>{})>
  node_dist closest_simplex(M const& mesh, P p) const noexcept {
    node_dist smallest = node_dist{};

    auto f = [&](auto&& n) {
      // If the aabb of the current node does not contain the point and the
      // distance to the aabb is larger than the currently computed distance,
      // the distance to any simplex inside the aabb cannot be smaller than the
      // current distance, so we are done:
      if (not hg::intersection.test(p, bounding_aabb(n))) {
        num_t d_aabb = distance(p, bounding_aabb(n));
        if (d_aabb > smallest.d) { return visitor_action::done; }
        // If the distance to the aabb is smaller, that does not mean that the
        // distance to a simplex contained in it will be smaller too.
      }

      // Otherwise, if the aabb is a leaf, compute the distance to its simplex
      // and update the minimum distance.
      if (is_leaf(n)) {
        num_t d = distance(p, mesh.simplex(node(n).simplex));
        if (d < smallest.d) { smallest = node_dist{n, d}; }
        return visitor_action::done;
      }

      // Otherwise, the aabb is not a leaf, that does contain the point. Need to
      // traverse to both children:
      return visitor_action::visit_both;
    };

    for_each(f, node_idx{0});
    return smallest;
  }

  /// Appends to the vector \p result the BVH nodes of the simplices whose
  /// bounding boxes intersect with \p target.
  template <typename T, typename Vector>
  void aabb_intersection(T const& target, Vector& result,
                         node_idx start = node_idx{0}) const {
    auto f = [&](auto&& n) {
      // If the bounding box of \p n does not intersect \p target we are done
      // (the bounding box of the children won't intersect target).
      if (not hg::intersection.test(bounding_aabb(n), target)) {
        return visitor_action::done;
      }

      // Intersect the bounding box but the node is not a leaf node, visit its
      // children:
      if (not is_leaf(n)) { return visitor_action::visit_both; }

      // The bounding box of \p n intersects \p target and n is a leaf node
      // containing a simplex, so we add the node to the vector and are done:
      HM3_ASSERT(node(n).simplex, "");
      result.push_back(n);
      return visitor_action::done;
    };

    for_each(f, start);
  }

  /// Appends to the vector \p result the ids of the simplices that intersect
  /// with \p target.
  template <typename M, typename T, typename Vector>
  void intersection(M const& mesh, T const& target, Vector& result,
                    node_idx start = node_idx{0}) const {
    auto f = [&](auto&& n) {
      // If the bounding box of \p n does not intersect \p target we are done
      // (the bounding box of the children won't intersect target).
      if (not hg::intersection.test(bounding_aabb(n), target)) {
        return visitor_action::done;
      }

      // Intersect the bounding box but the node is not a leaf node, visit its
      // children:
      if (not is_leaf(n)) { return visitor_action::visit_both; }

      // The bounding box of \p n intersects \p target and corresponds to that
      // of a simplex. If the simplex intersects the target, we add it.
      if (hg::intersection.test(mesh.simplex(n), target)) {
        result.push_back(n);
      }

      return visitor_action::done;
    };

    for_each(f, start);
  }

  /// How many simplices does the primitive \p p intersect?
  template <typename M, typename P>
  uint_t intersection_count(M const& mesh, P&& p) const noexcept {
    uint_t count = 0;

    auto f = [&](auto&& n) {
      if (not hg::intersection.test(p, bounding_aabb(n))) {
        return visitor_action::done;
      }

      if (is_leaf(n)) {
        if (hg::intersection.test(p, mesh.simplex(node(n).simplex))) {
          count += 1;
        }
        return visitor_action::done;
      }

      return visitor_action::visit_both;
    };

    for_each(f, node_idx{0});

    return count;
  }

  /// Returns a vector of BVH nodes whose bounding boxes intersect with the
  /// \p target.
  template <typename T>
  small_vector<node_idx, 128> aabb_intersection(T const& target) const {
    small_vector<node_idx, 128> result;
    aabb_intersection(target, result);
    return result;
  }

  /// Returns a vector of simplex ids that intersect the \p target.
  template <typename Mesh, typename T>
  small_vector<simplex_idx, 128> intersection(Mesh const& m,
                                              T const& target) const {
    small_vector<simplex_idx, 128> result;
    intersection(m, target, result);
    return result;
  }

  /// Does the primitive \p p intersect a simplex of the BVH?
  template <typename M, typename P>
  bool intersection_test(M const& mesh, P&& p) const noexcept {
    bool result = false;

    auto f = [&](auto&& n) {
      if (not hg::intersection.test(p, bounding_aabb(n))) {
        return visitor_action::done;
      }

      if (is_leaf(n)) {
        if (hg::intersection.test(p, mesh.simplex(node(n).simplex))) {
          result = true;
          return visitor_action::done;
        };
      }
      return visitor_action::visit_both;
    };

    for_each(f, node_idx{0});
    return result;
  }

  using count_t = optional_idx<uint_t, struct count_t>;

  /// Counts the number of non-degenerate intersections between the primitive
  /// \p p and the mesh simplices.
  template <typename M, typename P>
  count_t non_degenerate_intersection_count(M const& mesh, P&& p) const
   noexcept {
    count_t count{0};

    auto f = [&](auto&& n) {
      if (not count or not hg::intersection.test(p, bounding_aabb(n))) {
        return visitor_action::done;
      }

      if (is_leaf(n)) {
        non_degenerate_intersection_test_result r
         = hg::intersection.test_non_degenerate(p,
                                                mesh.simplex(node(n).simplex));
        switch (r) {
          case non_degenerate_intersection_test_result::intersection: {
            count += count_t{1};
            return visitor_action::done;
          }
          case non_degenerate_intersection_test_result::no_intersection: {
            return visitor_action::done;
          }
          case non_degenerate_intersection_test_result::
           degenerate_intersection: {
            count = count_t{};
            return visitor_action::done;
          }
        }
      }

      return visitor_action::visit_both;
    };

    for_each(f, node_idx{0});

    return count;
  }
};

}  // namespace bvh
}  // namespace hm3::geometry
