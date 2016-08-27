#pragma once
/// \file
///
/// Unique set of points
#include <hm3/geometry/algorithm/intersection/box_point.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/grid/hierarchical/cartesian/single.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/balanced_refine.hpp>
#include <hm3/utility/optional_idx.hpp>

namespace hm3::geometry {

/// Set of unique points
///
/// Backed up by an octree.
///
/// TODO: support deletion.
/// TODO: support mutating point coordinates.
template <dim_t Nd, suint_t MaxPointsPerNode = 9>
struct point_set {
  using point_t         = point<Nd>;
  using bbox_t          = box<Nd>;
  using tree_t          = grid::hierarchical::cartesian::single<Nd>;
  using point_storage   = vector<point_t>;
  using point_ids_t     = inline_vector<suint_t, MaxPointsPerNode>;
  using node_idx        = grid::hierarchical::tree_node_idx;
  using node_storage    = std::unordered_map<node_idx, point_ids_t>;
  using pidx_t          = suint_t;
  using optional_pidx_t = optional_idx<pidx_t, struct point_set_idx_t>;

  static constexpr suint_t max_points_per_node = MaxPointsPerNode;

 private:
  point_storage points_;
  node_storage nodes_;

  tree_t tree_;

 public:
  point_set() = default;

  /// Constructs a point set that can hold \p point_capacity points that must be
  /// located within the bounding box \p bbox.
  point_set(suint_t point_capacity, bbox_t bbox)
   : tree_(node_idx(point_capacity), bbox) {}

  /// Number of points in the point set.
  auto size() const noexcept { return points_.size(); }

 private:
  /// Asserts that the point \p p is inside the point-set's bounding box.
  void assert_point_in_bounding_box(point_t p) const noexcept {
    HM3_ASSERT(geometry::intersection.test(bounding_box(), p),
               "point {} is not inside the bounding box {}", p, bounding_box());
  }

  /// Adds point \p p to the set and returns its index within the set.
  ///
  /// This doesn't link the point to any particular tree node. The invariant
  /// that all points must be linked to a particular tree node must be resolved
  /// manually.
  ///
  /// \postcondition If the point is not in the set, `size()_after ==
  /// size()_before + 1`
  /// \postcodition If the point is already in the set `size()_after ==
  /// size()_before`.
  pidx_t append_point(point_t p) {
    assert_point_in_bounding_box(p);
    auto idx = points_.size();
    points_.push_back(p);
    return idx;
  }

  /// Adds point \p p to the set at the tree node \p n and returns its index
  /// within the set.
  ///
  /// \postcondition If the point is not in the set, `size()_after ==
  /// size()_before + 1`
  /// \postcodition If the point is already in the set `size()_after ==
  /// size()_before`.
  pidx_t append_point(point_t p, node_idx n) {
    assert_point_in_bounding_box(p);
    auto idx = append_point(p);
    nodes_[n].push_back(idx);
    return idx;
  }

  /// Grows the tree using a growth factor.
  ///
  /// Provides the strong exception safety guarantee. If growing the tree fails,
  /// no observable side-effects.
  void grow_tree() {
    static constexpr suint_t growth_factor = math::ipow(dim_t{2}, Nd);

    auto current_capacity = tree_.capacity();
    auto new_capacity     = growth_factor * current_capacity;

    // Copy the tree into a new tree with the new larger capacity:
    auto new_tree = tree_t(tree_, new_capacity);

    // Swap new tree on success with the old one:
    ranges::swap(tree_, new_tree);  // this cannot fail
  }

 public:
  /// Bounding box of of the point set, all points must be located inside the
  /// bounding box.
  bbox_t bounding_box() const noexcept { return tree_.bounding_box(); }

  /// Returns the point with index \p idx.
  point_t& point(pidx_t idx) noexcept {
    HM3_ASSERT(idx < points_.size(), "");
    return points_[idx];
  }

  /// Returns the point with index \p idx.
  point_t const& point(pidx_t idx) const noexcept {
    HM3_ASSERT(idx < points_.size(), "");
    return points_[idx];
  }

  /// Adds point \p p to the point-set and returns its index within the set.
  ///
  /// If the point is already within the set, it returns the index of the
  /// existing point, and `size()_after == size()_before`.
  ///
  /// If the point is not within the set, it inserts the point and returns its
  /// index, such that `size()_after == size()_before + 1`.
  ///
  /// \pre The point \p p must be located inside the point-set's bounding box.
  pidx_t push_back(point_t p) {
    assert_point_in_bounding_box(p);
    node_idx n = tree_.leaf_node_containing(p);
    HM3_ASSERT(n, "point {} in point-set's bbox {} but not in tree's bbox {}?",
               p, bounding_box(), tree_.bounding_box());
    bool checked_equal = false;
    while (true) {
      auto c = nodes_.count(n);
      // no points in node: add point
      if (c == 0) { return append_point(p, n); }
      // points in node
      auto& ids = nodes_[n];

      // if equal, return id of the already existing point
      if (!checked_equal) {
        // only need to check this once and not every time points are
        // redistributed to children (they are always the same points)
        for (auto&& id : ids) {
          auto other_p = points_[id];

          if (geometry::approx(p, other_p)) { return id; }
        }
        checked_equal = true;
      }
      // not equal, need to add the point

      // if enough capacity to add point at the node, just add the point
      if (ids.size() < ids.capacity()) { return append_point(p, n); }

      // not enough capacity to add the point at the current node
      //
      // refine the tree node, redestribute the points from parent to children,
      // and remove parent:
      auto new_children
       = tree::balanced_refine(tree_, n, [&](auto&& parent, auto&& children) {
           // TODO: maybe grow the tree?
           HM3_ASSERT(children,
                      "cannot refine node {}: size is {} and capacity is {}",
                      parent, tree_.size(), tree_.capacity());

           // if the tree node contains no points, nothing to do here
           if (nodes_.count(parent) == 0) { return; }
           // otherwise, redistribute points to children
           auto parent_ids = nodes_[parent];
           for (auto&& id : parent_ids) {
             auto node_point = points_[id];
             for (auto child : tree_.siblings(children)) {
               if (geometry::intersection.test(tree_.geometry(child),
                                               node_point)) {
                 nodes_[child].push_back(id);
               }
             }
           }
           nodes_.erase(parent);
         });

      // TODO: maybe grow the tree?
      HM3_ASSERT(new_children,
                 "cannot refine node {}: size is {} and capacity is {}", n,
                 tree_.size(), tree_.capacity());

      // move to the child that contains the point and repeat
      for (auto child : tree_.siblings(new_children)) {
        if (geometry::intersection.test(p, tree_.geometry(child))) {
          n = child;
          break;
        }
      }
      // in the next iteration n = child (recurse)
    }
  }

  /// Is the point \p in the set?
  optional_pidx_t contains(point_t p) const noexcept {
    // if not in the bounding box, return nothing:
    if (!geometry::intersection.test(p, tree_.bounding_box())) {
      return optional_pidx_t{};
    }
    auto n = tree_.leaf_node_containing(p);
    // if the node has no points, return nothing:
    if (nodes_.count(n) == 0) { return optional_pidx_t{}; }
    // if the node has points
    for (auto&& id : nodes_.at(n)) {
      // and one of them is p, return its id:
      if (geometry::approx(point(id), p)) { return optional_pidx_t{id}; }
    }
    // otherwise return nothing:
    return optional_pidx_t{};
  }

  /// Range of all points within the set.
  point_storage const& points() const noexcept { return points_; }
  /// Range of all points within the set.
  point_storage& points() noexcept { return points_; }
};

}  // namespace hm3::geometry
