#pragma once
/// \file grid.hpp Stores a solver grid in sync with the tree
#include <hm3/grid/grid.hpp>
#include <hm3/solver/state/types.hpp>

namespace hm3 {
namespace solver {
namespace state {

/// Stores a solver grid and maintains it in sync with the tree
///
/// For each solver grid node, it stores its corresponding tree node (if any),
/// and it also stores the solver node within the tree.
///
/// \note The solver grid is allowed to have holes in it.
/// \note Solver grid nodes do not necessarily need to be part of the grid
/// tree. For example ghost nodes might not exist within the tree.
template <uint_t nd> struct grid {
  using tree_t = ::hm3::grid::mhc<nd>;

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
  /// compact (i.e. there are no free nodes between the first and the last node)
  grid_node_idx size_ = 0_gn;
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
    HM3_ASSERT(
     (tn and in_tree(tn) == n) || !tn,
     "grid node {} has tree node {} but tree node {} has grid node {}", n, tn,
     tn, in_tree(tn));
#endif
    return tn;
  }
  /// Tree node of grid node \p n
  tree_node_idx tree_node(grid_node_idx n) const noexcept {
    assert_in_use(n, HM3_AT_);
    const auto tn = tree_node_ids_(n);
// In the paraview plugin the grid doesn't need to load the grid ids
#ifndef HM3_PARAVIEW_PLUGIN
    HM3_ASSERT(
     (tn and in_tree(tn) == n) || !tn,
     "grid node {} has tree node {} but tree node {} has grid node {}", n, tn,
     tn, in_tree(tn));
#endif
    return tn;
  }
  /// Parent tree node of grid node \p n
  tree_node_idx parent(grid_node_idx n) const noexcept {
    auto tn = tree_node(n);
    HM3_ASSERT(tn, "grid node {} is not part of the tree grid", n);
    return tree().parent(tn);
  }
  /// Children of grid node \p n (in tree nodes)
  auto children(grid_node_idx n) const noexcept {
    auto tn = tree_node(n);
    HM3_ASSERT(tn, "grid node {} is not part of the tree grid", n);
    return tree().children(tn);
  }

  /// Siblings of grid node \p n in tree
  auto tree_siblings(grid_node_idx n) const noexcept {
    auto tn = tree_node(n);
    HM3_ASSERT(tn, "grid node {} is not part of the tree grid", n);
    return tree().siblings(tn);
  }

  /// Siblings of grid node \p n in grid
  auto siblings(grid_node_idx n) const noexcept {
    return tree_siblings(n) | to_grid_nodes();
  }

  /// Neighbors of grid node \p n in tree
  auto tree_neighbors(grid_node_idx n) const noexcept {
    auto tn = tree_node(n);
    HM3_ASSERT(tn, "grid node {} is not part of the tree grid", n);
    return tree().neighbors(tn, idx());
  }

  /// Neighbors of grid node \p n in grid
  auto neighbors(grid_node_idx n) const noexcept {
    auto ns = tree_neighbors(n);
    stack::vector<grid_node_idx, ns.capacity()> gs(ns.size());
    RANGES_FOR (auto&& p, view::zip(ns, gs)) {
      std::get<1>(p) = in_tree(std::get<0>(p));
    }
    return gs;
  }

  /// Level of grid node \p n
  auto level(grid_node_idx n) const noexcept {
    auto tn = tree_node(n);
    HM3_ASSERT(tn, "grid node {} is not part of the tree grid", n);
    return tree().level(tn);
  }

  /// Asserts that the grid node \p n is within the capacity of the storage
  template <class At>
  void assert_within_capacity(grid_node_idx n, At&& at_) const noexcept {
    HM3_ASSERT_AT(n, "invalid grid node", at_);
    HM3_ASSERT_AT(n < capacity(),
                  "solver node '{}'  is out-of-capacity-bounds [0, {})", at_, n,
                  capacity());
  }

 private:
  /// Is the grid node \p n free?
  bool is_free(grid_node_idx n) const noexcept {
    assert_within_capacity(n, HM3_AT_);
    return is_free_(n);
  }

 public:
  /// Asserts that the grid node \p n is in use (i.e. it is not a free node)
  template <class At>
  void assert_in_use(grid_node_idx n, At&& at_) const noexcept {
    assert_within_capacity(n, at_);
    HM3_ASSERT_AT(!is_free_(n), "cell {} is not in use", at_, n);
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
    auto old_tn_i = tree_node_ids_(i);
    bool old_if_i = is_free_(i);
    auto old_gn_i = old_tn_i ? in_tree(old_tn_i) : grid_node_idx{};

    auto old_tn_j = tree_node_ids_(j);
    bool old_if_j = is_free_(j);
    auto old_gn_j = old_tn_j ? in_tree(old_tn_j) : grid_node_idx{};

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
      bool tmp = is_free_(i);
      is_free_(i) = is_free_(j);
      is_free_(j) = tmp;
    }

    ranges::swap(tree_node_ids_(i), tree_node_ids_(j));

    auto new_gn_i
     = tree_node_ids_(i) ? in_tree(tree_node_ids_(i)) : grid_node_idx{};
    auto new_gn_j
     = tree_node_ids_(j) ? in_tree(tree_node_ids_(j)) : grid_node_idx{};
    HM3_ASSERT(old_tn_i == tree_node_ids_(j), "");
    HM3_ASSERT(old_if_i == is_free_(j), "");
    HM3_ASSERT(old_gn_i == new_gn_j, "");

    HM3_ASSERT(old_tn_j == tree_node_ids_(i), "");
    HM3_ASSERT(old_if_j == is_free_(i), "");
    HM3_ASSERT(old_gn_j = new_gn_i, "");
  }

  /// Range of solver grid nodes (fast, requires contiguous solver nodes)
  auto operator()() const noexcept {
    HM3_ASSERT(is_compact(), "non-contiguous solver nodes. Either use "
                             "\"in_use()\" or make the container compact (e.g. "
                             "by sorting it).");
    return boxed_ints<grid_node_idx>(0_gn, size());
  }

  /// Range of solver grid nodes in use (slow, doesn't require contiguous nodes)
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

  auto to_grid_nodes() const noexcept {
    return view::transform(
     [&](tree_node_idx n) -> grid_node_idx { return in_tree(n); });
  }

  auto tree_nodes() const noexcept { return (*this)() | to_tree_nodes(); }

  /// Push grid node into solver nodes and get solver node
  grid_node_idx push(tree_node_idx n) {
    auto sn = free_node();
    HM3_ASSERT(is_free(sn), "node {} is not free", sn);
    if (sn >= capacity()) {
      /// TODO: this should probably throw to allow writing a checkpoint
      /// before dying
      HM3_TERMINATE("cannot push node: ran out of memory");
    }
    is_free_(sn) = false;

    if (n) {
      HM3_ASSERT(
       !in_tree(n),
       "tree node \"{}\" already has a valid grid node \"{}\" in grid \"{}\"",
       n, in_tree(n), idx());
      tree_node(sn) = n;
      in_tree(n) = sn;
    }  // else we push a new grid node without a connection with the tree
    ++size_;
    return sn;
  }

  /// Pop solver node \p sn
  ///
  /// \todo Provide a way to pop a sibling group (since this is typically what
  /// AMR does). Maybe one should provide this in grid::adaptor::multi as well.
  void pop(grid_node_idx sn) {
    assert_in_use(sn, HM3_AT_);
    auto gn = tree_node(sn);
    tree_node(sn) = tree_node_idx{};
    is_free_(sn) = true;
    if (gn) {
      HM3_ASSERT(in_tree(gn) == sn, "");
      tree().remove(gn, idx());
    }
    --size_;
    HM3_ASSERT(is_free(sn), "");
  }

  /// Coordinates of grid node \p n
  auto coordinates(grid_node_idx n) const noexcept {
    return tree().coordinates(tree_node(n));
  }

  /// RAII object for refining grid nodes
  ///
  /// When a grid node is refined, the children are allocated and this object
  /// returned. This object allows iterating over the grid children while the
  /// parent is still alive. When this object is destroyed, the parent grid node
  /// is remove from the grid;
  ///
  struct refine_t {
    grid& grid_;
    grid_node_idx parent_;
    refine_t(grid& g, grid_node_idx n) : grid_{g}, parent_{n} {}
    refine_t(refine_t&& other) : grid_{other.grid_}, parent_{other.parent_} {
      other.parent_ = grid_node_idx{};
    }
    refine_t operator=(refine_t&& other) = delete;
    refine_t(refine_t const&) = delete;
    refine_t operator=(refine_t const&) = delete;

    ~refine_t() {
      if (!parent_) { return; }
      grid_.pop(parent_);
    }
    auto operator()() const noexcept {
      return grid_.tree().children(grid_.tree_node(parent_))
             | grid_.tree().in_grid(grid_.idx()) | grid_.to_grid_nodes();
    }
  };

  /// Refine grid node \p n
  ///
  refine_t refine(grid_node_idx n) noexcept {
    auto tn = tree_node(n);
    HM3_ASSERT(tn, "grid node {} since it doesn't have a valid tree node", n);
    auto child_nodes = tree().refine(tn);
    if (distance(child_nodes) == 0) {
      HM3_TERMINATE("couldn't refine node {}", n);
    }
    for (auto&& i : child_nodes) {
      HM3_ASSERT(i, "invalid child node");
      push(i);
    }
    return {*this, n};
  }

  struct coarsen_t {
    grid& grid_;
    grid_node_idx parent_;
    coarsen_t(grid& g, grid_node_idx n) : grid_{g}, parent_{n} {}
    coarsen_t(coarsen_t&& other) : grid_{other.grid_}, parent_{other.parent_} {
      other.parent_ = grid_node_idx{};
    }
    ~coarsen_t() {
      if (!parent_) { return; }
      /// Pop the children:
      for (auto&& c : (*this)()) { grid_.pop(c); }
    }
    coarsen_t operator=(coarsen_t&& other) = delete;
    coarsen_t(coarsen_t const&) = delete;
    coarsen_t operator=(coarsen_t const&) = delete;

    auto operator()() const noexcept {
      return grid_.tree().children(grid_.tree_node(parent_))
             | grid_.tree().in_grid(grid_.idx()) | grid_.to_grid_nodes();
    }
  };

  /// Coarsens the parent of node \p n (i.e. the siblings of \p n)
  coarsen_t coarsen(grid_node_idx n) noexcept {
    auto p_tn = parent(n);
    auto p    = push(p_tn);
    return {*this, p};
  }

  grid() = default;

  grid(tree_t& t, grid_idx g, grid_node_idx node_capacity)
   : tree_{&t}
   , grid_idx_{g}
   , tree_node_ids_(*node_capacity)
   , is_free_(*node_capacity)
   , size_(0_gn) {
    reset();
  }

  grid(tree_t& t, grid_idx g, grid_node_idx node_capacity,
       hm3::log::serial& log)
   : grid(t, g, node_capacity) {
    log("Allocated memory for \"{}\" tree nodes", capacity());
  }

  /// Reset
  void reset() {
    size_ = 0_gn;
    is_free_.set();
    for (auto&& n : tree_node_ids_) { n = tree_node_idx{}; }
    // reset doesn't do the following, mainly because the grid already does it,
    // but also because the typical pattern is to read the grid first, and then
    // read the solver grid state, which mean the solver nodes are already in
    // the tree:
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
      auto cn = in_tree(n);
      tree_node_ids_(cn) = n;
    }
  }

  void update_tree() {
    RANGES_FOR (auto n, in_use()) {
      auto tn = tree_node(n);
      if (tn) { in_tree(tn) = n; }
    }
  }

  template <typename DataSwap> void sort(DataSwap&& ds) {
    auto i = grid_node_idx{0};
    for (auto n : tree().nodes(idx())) {
      auto cn = in_tree(n);
      ds(i, cn);
      swap(i, cn);
      ++i;
    }
  }
};

template <uint_t nd> bool operator==(grid<nd> const& a, grid<nd> const& b) {
  return a.idx() == b.idx() && a.size() == b.size()
         // compare the grids in the tree as well ?
         && equal(a.in_use(), b.in_use());
}
template <uint_t nd> bool operator!=(grid<nd> const& a, grid<nd> const& b) {
  return !(a == b);
}

/// \name Solver-grid I/O
///@{
template <uint_t nd> void map_arrays(io::file& f, grid<nd> const& g) {
  auto no_nodes = grid_node_idx{f.constant("no_grid_nodes", idx_t{})};
  HM3_ASSERT(no_nodes == g.size(), "mismatching number of grid nodes");
  f.field("tree_nodes", reinterpret_cast<const idx_t*>(g.data()), *no_nodes);
}

template <uint_t nd, typename tree_t = typename grid<nd>::tree_t>
grid<nd> from_file_unread(grid<nd> const&, io::file& f, tree_t& t,
                          grid_node_idx node_capacity) {
  auto idx = grid_idx{f.constant("grid_idx", suint_t{})};
  auto nd_ = uint_t{f.constant("spatial_dimension", suint_t{})};
  auto no_nodes = grid_node_idx{f.constant("no_grid_nodes", idx_t{})};
  if (nd_ != nd) {
    HM3_TERMINATE("spatial_dimension mismatch, type {} vs file {}", nd, nd_);
  }
  if (!node_capacity) { node_capacity = no_nodes; }
  grid<nd> g{t, idx, node_capacity};
  g.resize(no_nodes);
  map_arrays(f, g);
  return g;
}

template <uint_t nd> void to_file_unwritten(io::file& f, grid<nd> const& g) {
  HM3_ASSERT(g.is_compact(), "cannot write non-compact solver grid");
  f.field("grid_idx", *g.idx())
   .field("spatial_dimension", nd)
   .field("no_grid_nodes", *g.size());
  map_arrays(f, g);
}
///@}

}  // namespace state
}  // namespace solver
}  // namespace hm3
