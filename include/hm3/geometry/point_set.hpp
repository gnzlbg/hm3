#pragma once
/// \file
///
/// Unique set of points
#include <hm3/geometry/box.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/grid/hierarchical/cartesian/single.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/balanced_refine.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3 {
namespace geometry {

/// Set of unique points
///
/// Backed up by an octree.
///
/// TODO: support deletion.
template <dim_t Nd, suint_t MaxPointsPerNode = 9>  //
struct point_set {
  using point_t       = point<Nd>;
  using bbox_t        = box<Nd>;
  using tree_t        = grid::hierarchical::cartesian::single<Nd>;
  using point_storage = std::vector<point_t>;
  using point_ids_t   = inline_vector<suint_t, MaxPointsPerNode>;
  using node_idx      = grid::hierarchical::tree_node_idx;
  using node_storage  = std::unordered_map<node_idx, point_ids_t>;
  using pidx_t        = suint_t;

  static constexpr suint_t max_points_per_node = MaxPointsPerNode;

 private:
  point_storage points_;
  node_storage nodes_;

  tree_t tree_;
  num_t tolerance_;

 public:
  point_set(suint_t point_capacity, bbox_t bbox, num_t tolerance = 1e-12)
   : tree_(node_idx(point_capacity), bbox), tolerance_(tolerance) {}

  constexpr bool equal(point_t a, point_t b) const noexcept {
    for (dim_t d = 0; d < Nd; ++d) {
      if (std::abs(a(d) - b(d)) > tolerance_) { return false; }
    }
    return true;
  }

  auto size() const noexcept { return points_.size(); }

 private:
  pidx_t append_point(point_t p) {
    auto idx = points_.size();
    points_.push_back(p);
    return idx;
  }

  pidx_t append_point(point_t p, node_idx n) {
    auto idx = append_point(p);
    nodes_[n].push_back(idx);
    return idx;
  }

 public:
  bbox_t bounding_box() const noexcept { return tree_.bounding_box(); }

  point_t& point(pidx_t idx) noexcept {
    HM3_ASSERT(idx < points_.size(), "");
    return points_[idx];
  }

  point_t const& point(pidx_t idx) const noexcept {
    HM3_ASSERT(idx < points_.size(), "");
    return points_[idx];
  }

  pidx_t push_back(point_t p) {
    node_idx n         = tree_.leaf_node_containing(p);
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

          if (equal(p, other_p)) { return id; }
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
               if (geometry::contains(tree_.geometry(child), node_point)) {
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
        if (geometry::contains(tree_.geometry(child), p)) {
          n = child;
          break;
        }
      }
      // in the next iteration n = child (recurse)
    }
  }

  using optional_pidx_t = std2::experimental::optional<pidx_t>;

  /// Is the point \p in the set?
  optional_pidx_t contains(point_t p) {
    // if not in the bounding box, return nothing:
    if (!geometry::contains(tree_.bounding_box(), p)) {
      return optional_pidx_t{};
    }
    auto n = tree_.leaf_node_containing(p);
    // if the node has no points, return nothing:
    if (nodes_.count(n) == 0) { return optional_pidx_t{}; }
    // if the node has points
    for (auto&& id : nodes_[n]) {
      // and one of them is p, return its id:
      if (equal(point(id), p)) { return id; }
    }
    // otherwise return nothing:
    return optional_pidx_t{};
  }
};

}  // namespace geometry
}  // namespace hm3
