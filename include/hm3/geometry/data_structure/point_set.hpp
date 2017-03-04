#pragma once
/// \file
///
/// Unique set of points
#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/primitives.hpp>
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
template <typename P, suint_t MaxPointsPerNode = 9>
struct point_set {
  static constexpr dim_t Ad = ad_v<P>;
  using point_t             = P;
  using aabb_t              = associated::aabb_t<point_t>;
  using s_t                 = associated::edge_t<point_t>;
  using bbox_t              = associated::box_t<point_t>;
  using tree_t              = grid::hierarchical::cartesian::single<Ad>;
  using point_storage       = vector<point_t>;
  using point_ids_t         = fixed_capacity_vector<suint_t, MaxPointsPerNode>;
  using node_idx            = grid::hierarchical::tree_node_idx;
  using node_storage        = std::unordered_map<node_idx, point_ids_t>;
  using pidx_t              = suint_t;
  using optional_pidx_t     = optional_idx<pidx_t, struct point_set_idx_t>;

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
   : tree_(node_idx(point_capacity), bbox) {
    points_.reserve(point_capacity);
    nodes_.reserve(point_capacity);
  }

  /// Number of points in the point set.
  auto size() const noexcept { return points_.size(); }

  /// Is the point set empty?
  bool empty() const noexcept { return size() == 0_u; }

 private:
  /// Does the point set has a bounding box?
  bool has_bounding_box() const noexcept { return tree_.valid_bounding_box(); }

  /// Is the point \p p inside the bounding box of the set?
  bool is_point_in_bounding_box(point_t p) const noexcept {
    if (not has_bounding_box()) { return false; }
    return geometry::intersection.test(bounding_box(), p);
  }

  /// Asserts that the point \p p is inside the point-set's bounding box.
  void assert_point_in_bounding_box(point_t p) const noexcept {
    HM3_ASSERT(has_bounding_box(), "bounding-box is not set");
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
    static constexpr suint_t growth_factor = math::ipow(dim_t{2}, Ad);

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
  bbox_t bounding_box() const noexcept {
    if (not has_bounding_box()) { return bbox_t{}; }
    return tree_.bounding_box();
  }

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

  /// Capacity of the new tree in case of growth:
  uint_t next_tree_capacity() const noexcept {
    return (uint_t)math::ceil(1.2 * (num_t(*tree_.capacity()) + 10.));
  }

  void re_bound_tree(point_t p) {
    // If there is no bounding box, use bounding-box of the point:
    if (not has_bounding_box()) {
      HM3_ASSERT(empty(), "point set has no bounding-box but is not empty");
      point_set ps(next_tree_capacity(), bounding_volume.box(p));

      points_ = std::move(ps.points_);
      nodes_  = std::move(ps.nodes_);
      tree_   = std::move(ps.tree_);
      return;
    }

    /// Otherwise compute the new bounding box
    auto bbox    = bounding_box();
    point_t xmin = x_min(bbox), xmax = x_max(bbox);
    for (dim_t d = 0; d < Ad; ++d) {
      xmin(d) = std::min(xmin(d), p(d));
      xmax(d) = std::max(xmax(d), p(d));
    }
    auto new_bbox = bounding_volume.box(s_t{xmin, xmax});

    /// Create a new point set:
    point_set ps(next_tree_capacity(), new_bbox);

    /// Insert old points there
    for (auto& op : points()) { ps.push_back(op); }

    /// And replace this with that one
    points_ = std::move(ps.points_);
    nodes_  = std::move(ps.nodes_);
    tree_   = std::move(ps.tree_);
    // ranges::swap(*this, ps);
    return;
  }

  /// Adds point \p p to the point-set and returns its index within the set.
  ///
  /// If the point is already within the set, it returns the index of the
  /// existing point, and `size()_after == size()_before`.
  ///
  /// If the point is not within the set, it inserts the point and returns its
  /// index, such that `size()_after == size()_before + 1`.
  ///
  pidx_t push_back(point_t p) {
    /// If the point is not inside the bounding box:
    if (not is_point_in_bounding_box(p)) { re_bound_tree(p); }

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
    // Empty point sets contains no points
    if (empty()) { return optional_pidx_t{}; }
    HM3_ASSERT(has_bounding_box(), "non-empty point set has no bounding box");

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

  template <typename OStream>
  friend OStream& to_ascii(OStream& os, point_set const& ps,
                           ascii_fmt::geometry const&) {
    if (ps.empty()) {
      os << "{ point_set | empty }";
      return os;
    }

    os << "{ point_set | \n  bounding_box: ";
    ascii_fmt::to_ascii(os, ps.bounding_box());
    os << "\n  points:\n";

    suint_t idx = 0;
    for (auto&& p : ps.points()) {
      os << "    idx, " << idx << ", x: ";
      ascii_fmt::to_ascii(os, p);
      os << "\n";
      ++idx;
    }

    os << "  points at nodes:\n";
    for (auto& p : ps.nodes_) {
      os << "    nidx: ";
      ascii_fmt::to_ascii(os, p.first);
      os << ", ";
      ascii_fmt::to_ascii(os, p.second);
      os << ", b: ";
      ascii_fmt::to_ascii(os, ps.tree_.geometry(p.first));
      os << "\n";
    }

    os << "}";
    return os;
  }
};

}  // namespace hm3::geometry
