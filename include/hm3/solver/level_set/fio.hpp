#pragma once
/// \file fio.hpp
#include <hm3/solver/level_set/fwd.hpp>
#include <hm3/solver/state/grid.hpp>

namespace hm3 {
namespace solver {
namespace level_set {

/// \name Level-set  I/O
///@{

template <uint_t nd> void map_arrays(io::file& f, state<nd> const& s) {
  auto no_nodes = grid_node_idx{f.constant("no_grid_nodes", idx_t{})};
  HM3_ASSERT(no_nodes == s.g.size(), "mismatching number of grid nodes");
  f.field("signed_distance", s.signed_distance.data(), *no_nodes);
}

template <uint_t nd, typename tree_t = typename state<nd>::tree_t>
state<nd> from_file_unread(state<nd> const&, io::file& f, tree_t& t,
                           io::session& s_, grid_node_idx node_capacity) {
  auto g
   = from_file_unread(::hm3::solver::state::grid<nd>{}, f, t, node_capacity);
  state<nd> s(std::move(g), s_);
  map_arrays(f, s);
  return s;
}

template <uint_t nd> void to_file_unwritten(io::file& f, state<nd> const& s) {
  to_file_unwritten(f, s.g);
  map_arrays(f, s);
}

template <uint_t nd, typename tree_t = typename state<nd>::tree_t>
state<nd> from_file(state<nd> const&, io::file& f, tree_t& t, io::session& s,
                    grid_node_idx node_capacity = grid_node_idx{}) {
  auto ls = from_file_unread(state<nd>{}, f, t, s, node_capacity);
  f.read_arrays();
  return ls;
}

//@}

}  // namespace level_set
}  // namespace solver
}  // namespace hm3
