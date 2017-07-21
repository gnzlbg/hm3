#pragma once
/// \file
///
/// Level-set solver state
#include <hm3/io/client.hpp>
#include <hm3/solver/level_set/fio.hpp>
#include <hm3/solver/level_set/fwd.hpp>
#include <hm3/solver/utility/base.hpp>

namespace hm3::solver::level_set {

/// Solver-state type-name
template <dim_t Ad>
string type(state<Ad> const&) {
  return "level_set";
}

template <dim_t Ad>
struct state : geometry::with_ambient_dimension<Ad>, base {
  using grid = ::hm3::grid::hierarchical::client::multi<Ad>;

  using tree_t   = typename grid::tree_t;
  using cell_idx = grid_node_idx;
  using node_idx = tree_node_idx;

  grid g;

  dense::vector<num_t, dense::dynamic, cell_idx> signed_distance;

  state() = default;

  /// Constructs a new empty state
  state(tree_t& t, grid_idx gidx, grid_node_idx node_capacity)
   : base(t.client().session(), gidx, *this, t)
   , g(t, gidx, node_capacity, log)
   , signed_distance(*node_capacity) {
    reset();
  }

  state(grid&& g_)
   : base(g_.client().session(), g_.idx(), *this, g.tree())
   , g(std::move(g_))
   , signed_distance(*g.capacity()) {}

  state(state&&) = default;
  state& operator=(state&&) = default;

  auto idx() const noexcept { return g.idx(); }

  /// Reset level-set solver state
  void reset() noexcept {
    for (auto&& i : signed_distance) { i = math::highest<num_t>; }
    g.reset();
  }

  /// Inserts tree node \p n into the solver grid
  cell_idx push(node_idx n) noexcept { return g.push(n); };

  /// Removes the solver cell \p c from the solver grid
  void pop(cell_idx i) noexcept { g.pop(i); }

  template <typename ChildrenRange>
  void interpolate_from_parent_to_children(cell_idx parent,
                                           ChildrenRange&& children) noexcept {
    for (auto&& child : children) {
      signed_distance(child) = signed_distance(parent);
    }
  }

  /// Refines the solver cell \p c and returns a range of the newly created
  /// children
  ///
  /// \post The cell \p c is removed from the grid
  auto refine(const cell_idx c) noexcept {
    // parent will be deleted at the end of this scope
    auto refine_guard = g.refine(c);
    interpolate_from_parent_to_children(c, refine_guard.new_children());
    return refine_guard.new_children();
  }

  template <typename ChildrenRange>
  void interpolate_from_children_to_parent(cell_idx parent,
                                           ChildrenRange&& children) noexcept {
    suint_t count           = 0;
    signed_distance(parent) = 0;
    for (auto&& child : children) {
      signed_distance(parent) += signed_distance(child);
      ++count;
    }
    HM3_ASSERT(count != 0, "count cannot be equal to zero");
    signed_distance(parent) /= static_cast<num_t>(count);
  }

  /// Coarsens the sibling grid nodes of \p and returns the parent cell id
  cell_idx coarsen(const cell_idx n) noexcept {
    // children will be deleted at the end of this scope
    auto coarsen_guard = g.coarsen(n);
    interpolate_from_children_to_parent(coarsen_guard.new_parent(),
                                        coarsen_guard.old_children());
    for (auto i : coarsen_guard.old_children()) {
      signed_distance(i) = math::lowest<num_t>;
    }
    return coarsen_guard.new_parent();
  }

  template <typename SD>
  void set_node_values(SD&& sd) noexcept {
    for (auto&& n : g.in_use()) { signed_distance(n) = sd(g.coordinates(n)); }
  }

  void write() {
    auto f = io_.new_file();
    to_file_unwritten(f, *this);
    io_.write(f);
  }

  void sort() {
    HM3_ASSERT(g.tree().is_sorted(), "tree is not sorted!");

    // update solver ids to point to the new tree nodes
    g.update_from_tree();
    g.sort([&](cell_idx i, cell_idx j) {
      std::swap(signed_distance(i), signed_distance(j));
    });
    g.update_tree();

    HM3_ASSERT(g.is_compact(), "??");
  }

  auto geometry(cell_idx n) const noexcept {
    return g.tree().geometry(g.tree_node(n));
  }
  auto bounding_box() const noexcept { return g.tree().bounding_box(); }

  static state<Ad> from_session(io::session& s, tree_t& t, string name_,
                                io::file::index_t i) {
    io::client io_(s, name_, type(state<Ad>{}), name(t));
    auto f = io_.get_file(i);
    return from_file(state<Ad>{}, f, t, s);
  }
};

template <dim_t Ad>
bool operator==(state<Ad> const& a, state<Ad> const& b) {
  return a.g == b.g and equal(a.signed_distance, b.signed_distance);
}

template <dim_t Ad>
bool operator!=(state<Ad> const& a, state<Ad> const& b) {
  return !(a == b);
}

}  // namespace hm3::solver::level_set
