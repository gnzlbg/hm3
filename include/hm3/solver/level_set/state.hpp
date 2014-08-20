#pragma once
/// \file
///
/// Level-set solver state
#include <hm3/io/client.hpp>
#include <hm3/solver/level_set/fwd.hpp>
#include <hm3/solver/level_set/fio.hpp>

namespace hm3 {
namespace solver {
namespace level_set {

/// Solver-state type-name
template <uint_t Nd> string type(state<Nd> const&) { return "level_set"; }

/// Name of the level-set solver state
template <uint_t Nd> string name(state<Nd> const& s, grid_idx idx) {
  using std::to_string;
  return type(s) + "_" + to_string(*idx);
}

template <uint_t Nd> string name(state<Nd> const& s) {
  return name(s, s.idx());
}

template <uint_t Nd> struct state {
  using grid = ::hm3::solver::state::grid<Nd>;

  using tree_t   = typename grid::tree_t;
  using cell_idx = grid_node_idx;
  using node_idx = tree_node_idx;

  hm3::log::serial log;
  grid g;
  io::client io_;

  dense::vector<num_t, dense::dynamic, cell_idx> signed_distance;

  state() = default;

  /// Constructs a new empty state
  state(tree_t& t, grid_idx gidx, grid_node_idx node_capacity, io::session& s)
   : log(name(*this, gidx))
   , g(t, gidx, node_capacity, log)
   , io_(s, name(*this), type(*this), name(g.tree()))
   , signed_distance(*node_capacity) {
    reset();
  }

  state(grid&& g_, io::session& s)
   : log(name(*this, g_.idx()))
   , g(std::move(g_))
   , io_(s, name(*this), type(*this), name(g.tree()))
   , signed_distance(*g.capacity()) {}

  state(state&&) = default;
  state& operator=(state&&) = default;

  auto idx() const noexcept { return g.idx(); }

  /// Reset level-set solver state
  void reset() noexcept {
    for (auto&& i : signed_distance) { i = std::numeric_limits<num_t>::max(); }
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

  /// Refines the solver cell \p c
  auto refine(const cell_idx i) noexcept {
    // parent will be deleted at the end of this scope
    auto guard = g.refine(i);
    interpolate_from_parent_to_children(i, guard());
    return guard();  // returns range of newly created children
  }

  template <typename ChildrenRange>
  void interpolate_from_children_to_parent(cell_idx parent,
                                           ChildrenRange&& children) noexcept {
    uint_t count = 0;
    signed_distance(parent) = 0;
    for (auto&& child : children) {
      signed_distance(parent) += signed_distance(child);
      ++count;
    }
    HM3_ASSERT(count != 0, "count cannot be equal to zero");
    signed_distance(parent) /= count;
  }

  /// Coarsens the sibling grid nodes of \p n
  cell_idx coarsen(const cell_idx n) noexcept {
    // children will be deleted at the end of this scope
    auto guard = g.coarsen(n);
    interpolate_from_children_to_parent(n, guard());
    for (auto i : guard()) { signed_distance(i) = -1; }
    return guard.parent_;  // retuns cell_idx of parent
  }

  template <typename SD> void set_node_values(SD&& sd) noexcept {
    RANGES_FOR (auto&& n, g.in_use()) {
      signed_distance(n) = sd(g.coordinates(n));
    }
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
  auto dimensions() const noexcept { return g.tree().dimensions(); }

  static state<Nd> from_session(io::session& s, tree_t& t, string name_,
                                io::file::index_t i) {
    io::client io_(s, name_, type(state<Nd>{}), name(t));
    auto f = io_.get_file(i);
    return from_file(state<Nd>{}, f, t, s);
  }
};

template <uint_t Nd> bool operator==(state<Nd> const& a, state<Nd> const& b) {
  return a.g == b.g and equal(a.signed_distance, b.signed_distance);
}

template <uint_t Nd> bool operator!=(state<Nd> const& a, state<Nd> const& b) {
  return !(a == b);
}

}  // namespace level_set
}  // namespace solver
}  // namespace hm3
