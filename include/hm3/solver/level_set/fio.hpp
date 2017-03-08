#pragma once
/// \file
///
/// Serialization to HM3's File I/O
#include <hm3/solver/level_set/fwd.hpp>

namespace hm3::solver::level_set {

/// \name Level-set  I/O
///@{

template <dim_t Ad>
void map_arrays(io::file& f, state<Ad> const& s) {
  auto no_nodes = grid_node_idx{f.constant("no_grid_nodes", idx_t{})};
  HM3_ASSERT(no_nodes == s.g.size(), "mismatching number of grid nodes");
  f.field("signed_distance", s.signed_distance.data(), *no_nodes);
}

template <dim_t Ad, typename Tree = typename state<Ad>::tree_t>
state<Ad> from_file_unread(state<Ad> const&, io::file& f, Tree& t,
                           io::session& s_, grid_node_idx node_capacity) {
  auto g = from_file_unread(grid_client_t<Ad>{}, f, t, node_capacity);
  state<Ad> s(std::move(g), s_);
  map_arrays(f, s);
  return s;
}

template <dim_t Ad>
void to_file_unwritten(io::file& f, state<Ad> const& s) {
  to_file_unwritten(f, s.g);
  map_arrays(f, s);
}

template <dim_t Ad, typename Tree = typename state<Ad>::tree_t>
state<Ad> from_file(state<Ad> const&, io::file& f, Tree& t, io::session& s,
                    grid_node_idx node_capacity = grid_node_idx{}) {
  auto ls = from_file_unread(state<Ad>{}, f, t, s, node_capacity);
  f.read_arrays();
  return ls;
}

//@}

}  // namespace hm3::solver::level_set
