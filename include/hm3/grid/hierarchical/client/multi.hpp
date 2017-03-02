#pragma once
/// \file
///
/// Stores a single grid within a hierarchical Cartesian multi-tree.
#include <hm3/grid/hierarchical/cartesian/multi.hpp>
#include <hm3/grid/hierarchical/grid.hpp>

namespace hm3 {
namespace grid {
namespace hierarchical {
namespace client {

/// Client to a single grid stored within a hierarchical Cartesian multi-tree.
///
/// \tparam Ad number of spatial dimensions of the grid
///
/// For each grid node, it stores its corresponding tree node (if any), and it
/// also stores the grid node within the tree.
///
/// \note The grid is allowed to have holes in it.
///
/// \note Grid nodes do not necessarily need to be part of the grid tree. For
/// example ghost nodes might not exist within the tree.
template <dim_t Ad>
struct multi : geometry::with_ambient_dimension<Ad> {
  using self   = multi<Ad>;
  using tree_t = cartesian::multi<Ad>;

 private:
  using tree_node_ids
   = dense::vector<tree_node_idx, dense::dynamic, grid_node_idx>;
  using bit_vector = dense::vector<dense::bit, dense::dynamic, grid_node_idx>;

  /// \name Data members
  ///@{
  /// A reference to the tree grid
  tree_t* tree_;
  /// Index of the solver grid within the tree
  grid_idx grid_idx_;
  /// Stores the tree nodes of each grid node
  tree_node_ids tree_node_ids_;
  /// Stores if a grid node is free or in use (allows holes)
  bit_vector is_free_;
  /// Number of grid nodes currently stored.
  ///
  /// This is equivalent to one past the last node stored if the nodes are
  /// compact (i.e. there are no free nodes between the first and the last
  /// node)
  grid_node_idx size_ = 0_gn;
  /// Smallest level in the grid
  level_idx min_level_;
  /// Largest level in the grid.
  level_idx max_level_;
  ///@}  // Data members

 public:
  /// Maximum number of grid nodes that can be stored
  grid_node_idx capacity() const noexcept { return tree_node_ids_.size(); }

  /// Number of grid nodes stored
  grid_node_idx size() const noexcept { return size_; }

  /// Access the tree
  tree_t const& tree() const noexcept { return *tree_; }
  /// Access the tree
  tree_t& tree() noexcept { return *tree_; }

  /// Solver grid idx
  grid_idx idx() const noexcept { return grid_idx_; }

  /// Solver grid data
  auto data() const noexcept { return tree_node_ids_.data(); }

 private:
  template <typename At>
  static void assert_valid(tree_node_idx n, At&& at) noexcept {
    HM3_ASSERT_AT(n, "invalid node", std::forward<At>(at));
  }

 public:
  /// Solver grid node at tree node \p i
  grid_node_idx in_tree(tree_node_idx i) const noexcept {
    assert_valid(i, HM3_AT_);
    return tree().node(i, idx());
  }
  /// Solver grid node index at tree node \p i
  grid_node_idx& in_tree(tree_node_idx i) noexcept {
    assert_valid(i, HM3_AT_);
    return tree().node(i, idx());
  }

  /// Tree node of grid node \p n
  tree_node_idx& tree_node(grid_node_idx n) noexcept {
    assert_in_use(n, HM3_AT_);
    auto& tn = tree_node_ids_(n);
// In the paraview plugin the grid doesn't need to load the grid ids
#ifndef HM3_PARAVIEW_PLUGIN
    HM3_ASSERT((tn and in_tree(tn) == n) || !tn,
               "grid node {} has tree node "
               "{} but tree node {} has grid "
               "node {} on grid {}",
               n, tn, tn, in_tree(tn), idx());
#endif
    return tn;
  }
  /// Tree node of grid node \p n
  tree_node_idx tree_node(grid_node_idx n) const noexcept {
    assert_in_use(n, HM3_AT_);
    const auto tn = tree_node_ids_(n);
// In the paraview plugin the grid doesn't need to load the grid ids
#ifndef HM3_PARAVIEW_PLUGIN
    HM3_ASSERT((tn and in_tree(tn) == n) || !tn,
               "grid node {} has tree node "
               "{} but tree node {} has grid "
               "node {} on grid {}",
               n, tn, tn, in_tree(tn), idx());
#endif
    return tn;
  }
  /// Parent tree node of grid node \p n
  tree_node_idx parent(grid_node_idx n) const noexcept {
    auto tn = tree_node(n);
    HM3_ASSERT(tn, "grid node {} is not part of the tree grid {}", n, idx());
    return tree().parent(tn);
  }
  /// Children of grid node \p n (in tree nodes)
  auto children(grid_node_idx n) const noexcept {
    auto tn = tree_node(n);
    HM3_ASSERT(tn, "grid node {} is not part of the tree grid {}", n, idx());
    return tree().children(tn);
  }

  /// Siblings of grid node \p n in tree
  auto tree_siblings(grid_node_idx n) const noexcept {
    auto tn = tree_node(n);
    HM3_ASSERT(tn, "grid node {} is not part of the tree grid {}", n, idx());
    return tree().siblings(tn);
  }

  /// Siblings of grid node \p n in grid
  auto siblings(grid_node_idx n) const noexcept {
    return tree_siblings(n) | to_grid_nodes();
  }

  /// Neighbors of grid node \p n in tree
  auto tree_node_neighbors_in_tree(grid_node_idx n) const noexcept {
    auto tn = tree_node(n);
    HM3_ASSERT(tn, "grid node {} is not part of the tree grid {}", n, idx());
    return tree().neighbors(tn, idx());
  }

  /// Neighbors of grid node \p n in grid
  auto grid_node_neighbors_in_tree(grid_node_idx n) const noexcept {
    auto ns = tree_node_neighbors_in_tree(n);
    fixed_capacity_vector<grid_node_idx, ns.capacity()> gs(ns.size());
    for (auto&& p : view::zip(ns, gs)) {
      std::get<1>(p) = in_tree(std::get<0>(p));
    }
    return gs;
  }

  /// Level of grid node \p n
  auto level(grid_node_idx n) const noexcept {
    auto tn = tree_node(n);
    HM3_ASSERT(tn, "grid node {} is not part of the tree grid {}", n, idx());
    return tree().level(tn);
  }

  /// Asserts that the grid node \p n is within the capacity of the storage
  template <typename At>
  void assert_within_capacity(grid_node_idx n, At&& at_) const noexcept {
    HM3_ASSERT_AT(n, "invalid grid node", std::forward<At>(at_));
    HM3_ASSERT_AT(
     n < capacity(),
     "solver node '{}' of grid {} is out-of-capacity-bounds [0, {})",
     std::forward<At>(at_), n, idx(), capacity());
  }

 private:
  /// Is the grid node \p n free?
  bool is_free(grid_node_idx n) const noexcept {
    assert_within_capacity(n, HM3_AT_);
    return is_free_(n);
  }

 public:
  /// Asserts that the grid node \p n is in use (i.e. it is not a free node)
  template <typename At>
  void assert_in_use(grid_node_idx n, At&& at_) const noexcept {
    assert_within_capacity(n, at_);
    HM3_ASSERT_AT(!is_free_(n), "cell {} of grid {} is not in use",
                  std::forward<At>(at_), n, idx());
  }

 private:
  /// Returns the position of a free grid node
  grid_node_idx free_node() const noexcept {
    // auto i = is_free_.find_next(hint);
    auto i = is_free_.find_first();
    return i == is_free_.npos() ? grid_node_idx{} : grid_node_idx{i};
  }

 public:
  /// Are the grid nodes contiguous
  bool is_compact() const noexcept {
    return !free_node() or free_node() == size();
  }

  /// Swaps the positions of two grid nodes
  ///
  /// This swaps their position in the tree_node_ids_ container, it swaps the
  /// is_free_, and it updates the back links in the tree to to their new
  /// positions.
  void swap(grid_node_idx i, grid_node_idx j) {
    assert_within_capacity(i, HM3_AT_);
    assert_within_capacity(j, HM3_AT_);
    HM3_ASSERT((is_free(j) and !tree_node_ids_(j))
                or (!is_free(j) and tree_node_ids_(j)),
               "");
    HM3_ASSERT((is_free(i) and !tree_node_ids_(i))
                or (!is_free(i) and tree_node_ids_(i)),
               "");

    if (tree_node_ids_(i) && tree_node_ids_(j)) {
      ranges::swap(in_tree(tree_node_ids_(i)), in_tree(tree_node_ids_(j)));
    } else if (tree_node_ids_(i)) {
      in_tree(tree_node_ids_(i)) = j;
    } else if (tree_node_ids_(j)) {
      in_tree(tree_node_ids_(j)) = i;
    }
    {
      bool tmp    = is_free_(i);
      is_free_(i) = is_free_(j);
      is_free_(j) = tmp;
    }

    ranges::swap(tree_node_ids_(i), tree_node_ids_(j));
  }

  /// Range of solver grid nodes (fast, requires contiguous solver nodes)
  auto operator()() const noexcept {
    HM3_ASSERT(is_compact(), "non-contiguous solver nodes. Either use "
                             "\"in_use()\" or make the container compact (e.g. "
                             "by sorting it).");
    return boxed_ints<grid_node_idx>(0_gn, size());
  }

  /// Range of solver grid nodes in use (slow, doesn't require contiguous
  /// nodes)
  auto in_use() const noexcept {
    return boxed_ints<grid_node_idx>(0_gn, capacity())
           | view::filter([&](grid_node_idx n) { return !is_free(n); })
           | view::take(*size());
  }

  /// Transform a range of grid nodes into a range of tree nodes
  auto to_tree_nodes() const noexcept {
    return view::transform(
     [&](grid_node_idx n) -> tree_node_idx { return tree_node(n); });
  }

  /// Transform a range of tree nodes into a range of grid nodes
  auto to_grid_nodes() const noexcept {
    return view::transform(
     [&](tree_node_idx n) -> grid_node_idx { return in_tree(n); });
  }

  /// Tree nodes of all noes in the grid
  auto tree_nodes() const noexcept { return (*this)() | to_tree_nodes(); }

  /// Range of levels of nodes in the tree: [min_level, max_level]
  auto levels() const noexcept {
    return min_level_ and max_level_
            ? boxed_ints<level_idx>(min_level_, max_level_ + 1)
            : boxed_ints<level_idx>(0, 0);
  }

 private:
  void update_minmax_level(tree_node_idx n) noexcept {
    HM3_ASSERT(n, "");
    auto l     = tree().level(n);
    min_level_ = min_level_ ? std::min(min_level_, l) : l;
    max_level_ = max_level_ ? std::max(max_level_, l) : l;
  }

 public:
  /// Push grid node into solver nodes and get solver node
  grid_node_idx push(tree_node_idx n) {
    auto sn = free_node();
    HM3_ASSERT(is_free(sn), "node {} of grid {} is not free", sn, idx());
    if (sn >= capacity()) {
      /// TODO: this should probably throw to allow writing a checkpoint
      /// before dying
      HM3_FATAL_ERROR("cannot push node to grid {}: ran out of memory", idx());
    }
    is_free_(sn) = false;

    if (n) {
      HM3_ASSERT(
       !in_tree(n),
       "tree node \"{}\" already has a valid grid node \"{}\" in grid \"{}\"",
       n, in_tree(n), idx());
      tree_node(sn) = n;
      in_tree(n)    = sn;
      update_minmax_level(n);
    }  // else we push a new grid node without a connection with the tree
    ++size_;
    return sn;
  }

  /// Pop solver node \p sn
  ///
  /// \todo Provide a way to pop a sibling group (since this is typically what
  /// AMR does). Maybe one should provide this in grid::adaptor::multi as
  /// well.
  void pop(grid_node_idx sn) {
    assert_in_use(sn, HM3_AT_);
    auto gn       = tree_node(sn);
    tree_node(sn) = tree_node_idx{};
    is_free_(sn)  = true;
    if (gn) {
      HM3_ASSERT(in_tree(gn) == sn,
                 "link from tree to grid is broken: gn = {} "
                 "=> in_tree({}) = {} != sn = {}",
                 gn, gn, in_tree(gn), sn);
      tree().remove(gn, idx());
    }
    --size_;
    HM3_ASSERT(is_free(sn), "");
  }

  /// Coordinates of grid node \p n
  auto coordinates(grid_node_idx n) const noexcept {
    auto tn = tree_node(n);
    HM3_ASSERT(tn,
               "not implemented: node {} of grid {} has no tree node so cannot "
               "compute coordinates here",
               n, idx());
    return tree().coordinates(tn);
  }

  /// RAII object for refining grid nodes
  ///
  /// When a grid node is refined, the children are allocated and this object
  /// returned. This object allows iterating over the grid children while the
  /// parent is still alive. When this object is destroyed, the parent grid
  /// node
  /// is remove from the grid;
  ///
  struct refine_t {
    self& grid_;
    grid_node_idx parent_;
    refine_t(self& g, grid_node_idx n) : grid_{g}, parent_{n} {}
    refine_t(refine_t&& other) : grid_{other.grid_}, parent_{other.parent_} {
      other.parent_ = grid_node_idx{};
    }
    refine_t operator=(refine_t&& other) = delete;
    refine_t(refine_t const&)            = delete;
    refine_t operator=(refine_t const&) = delete;

    ~refine_t() {
      if (!parent_) { return; }
      grid_.pop(parent_);
    }
    auto new_children() const noexcept {
      return grid_.tree().children(grid_.tree_node(parent_))
             | grid_.tree().in_grid(grid_.idx()) | grid_.to_grid_nodes();
    }
    auto old_parent() const noexcept { return parent_; }
  };

  /// Refine grid node \p n
  ///
  refine_t refine(grid_node_idx n) noexcept {
    auto tn = tree_node(n);
    HM3_ASSERT(tn, "grid node {} of grid {} doesn't have a valid tree node", n,
               idx());
    auto child_nodes = tree().refine(tn);
    if (ranges::distance(child_nodes) == 0) {
      HM3_FATAL_ERROR("couldn't refine node {} on grid {}", n, idx());
    }
    for (auto&& i : child_nodes) {
      HM3_ASSERT(i, "invalid child node in grid {}", idx());
      push(i);
    }
    return {*this, n};
  }

  struct coarsen_t {
    self& grid_;
    grid_node_idx parent_;
    coarsen_t(self& g, grid_node_idx n) : grid_{g}, parent_{n} {}
    coarsen_t(coarsen_t&& other) : grid_{other.grid_}, parent_{other.parent_} {
      other.parent_ = grid_node_idx{};
    }
    ~coarsen_t() {
      if (!parent_) { return; }
      /// Pop the children:
      for (auto&& c : old_children()) { grid_.pop(c); }
    }
    coarsen_t operator=(coarsen_t&& other) = delete;
    coarsen_t(coarsen_t const&)            = delete;
    coarsen_t operator=(coarsen_t const&) = delete;

    auto old_children() const noexcept {
      return grid_.tree().children(grid_.tree_node(parent_))
             | grid_.tree().in_grid(grid_.idx()) | grid_.to_grid_nodes();
    }
    auto new_parent() const noexcept { return parent_; }
  };

  /// Coarsens the parent of node \p n (i.e. the siblings of \p n)
  coarsen_t coarsen(grid_node_idx n) noexcept {
    auto p_tn = parent(n);
    auto p    = push(p_tn);
    return {*this, p};
  }

  template <typename Projection>
  void refine(grid_node_idx n, Projection&& projection) {
    auto refine_guard = refine(n);
    projection(refine_guard.old_parent(), refine_guard.new_children());
  }

  template <typename Restriction>
  void coarsen(grid_node_idx n, Restriction&& restriction) {
    auto coarsen_guard = coarsen(n);
    restriction(coarsen_guard.new_parent(), coarsen_guard.old_children());
  }

  multi() = default;

  multi(tree_t& t, grid_idx g, grid_node_idx node_capacity)
   : tree_{&t}
   , grid_idx_{g}
   , tree_node_ids_(*node_capacity)
   , is_free_(*node_capacity)
   , size_(0_gn) {
    reset();
  }

  multi(tree_t& t, grid_idx g, grid_node_idx node_capacity,
        hm3::log::serial& log)
   : multi(t, g, node_capacity) {
    log("Allocated memory for \"{}\" tree nodes", capacity());
  }

  /// Reset
  void reset() {
    size_ = 0_gn;
    is_free_.set();
    for (auto&& n : tree_node_ids_) { n = tree_node_idx{}; }
    min_level_ = level_idx{};
    max_level_ = level_idx{};
    // reset doesn't do the following, mainly because the grid already does
    // it, but also because the typical pattern is to read the grid first, and
    // then read the solver grid state, which mean the solver nodes are
    // already in the tree:
    for (auto&& n : tree().nodes(idx())) { in_tree(n) = grid_node_idx{}; }
  }

  /// Resizes a zero sized grid to have \p s grid nodes
  void resize(grid_node_idx s) noexcept {
    HM3_ASSERT(size_ == 0_gn, "cannot resize non empty grid, size = {}", size_);
    size_ = s;
    for (auto&& n : boxed_ints<grid_node_idx>(0_gn, size_)) {
      is_free_(n) = false;
    }
    HM3_ASSERT(size() == s, "");
    HM3_ASSERT(is_compact(), "size is {} but first free node is {}", size(),
               free_node());
  }

  void update_from_tree() {
    for (auto n : tree().nodes(idx())) {
      auto cn            = in_tree(n);
      tree_node_ids_(cn) = n;
    }
  }

  void update_tree() {
    for (auto n : in_use()) {
      auto tn = tree_node(n);
      if (tn) { in_tree(tn) = n; }
    }
  }

  template <typename DataSwap>
  void sort(DataSwap&& ds) {
    min_level_ = level_idx{};
    max_level_ = level_idx{};
    auto i     = grid_node_idx{0};
    for (auto n : tree().nodes(idx())) {
      auto cn = in_tree(n);
      ds(i, cn);
      swap(i, cn);
      ++i;
      update_minmax_level(n);
    }
  }

  friend bool operator==(self const& a, self const& b) {
    return a.idx() == b.idx() && a.size() == b.size() && a.tree_ == b.tree_
           && equal(a.in_use(), b.in_use());
  }
};

template <dim_t Ad>
bool operator!=(multi<Ad> const& a, multi<Ad> const& b) {
  return !(a == b);
}

/// \name Solver-grid I/O
///@{
template <dim_t Ad>
void map_arrays(io::file& f, multi<Ad> const& g) {
  auto no_nodes = grid_node_idx{f.constant("no_grid_nodes", idx_t{})};
  HM3_ASSERT(no_nodes == g.size(), "mismatching number of grid nodes");
  f.field("tree_nodes", reinterpret_cast<const idx_t*>(g.data()), *no_nodes);
}

template <dim_t Ad, typename Tree = typename multi<Ad>::tree_t>
multi<Ad> from_file_unread(multi<Ad> const&, io::file& f, Tree& t,
                           grid_node_idx node_capacity) {
  auto idx      = grid_idx{f.constant("grid_idx", hierarchical::gidx_t{})};
  auto nd       = dim_t{f.constant("spatial_dimension", dim_t{})};
  auto no_nodes = grid_node_idx{f.constant("no_grid_nodes", idx_t{})};
  if (Ad != nd) {
    HM3_FATAL_ERROR("spatial_dimension mismatch, type {} vs file {}", Ad, nd);
  }
  if (!node_capacity) { node_capacity = no_nodes; }
  multi<Ad> g{t, idx, node_capacity};
  g.resize(no_nodes);
  map_arrays(f, g);
  return g;
}

template <dim_t Ad>
void to_file_unwritten(io::file& f, multi<Ad> const& g) {
  HM3_ASSERT(g.is_compact(), "cannot write non-compact solver grid");
  f.field("grid_idx", *g.idx())
   .field("spatial_dimension", Ad)
   .field("no_grid_nodes", *g.size());
  map_arrays(f, g);
}
///@}

}  // namespace client
}  // namespace hierarchical
}  // namespace grid
}  // namespace hm3
