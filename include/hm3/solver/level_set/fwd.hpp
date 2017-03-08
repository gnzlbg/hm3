#pragma once
/// \file
///
/// Forward declarations of level-set solver
#include <hm3/grid/hierarchical/client/multi.hpp>
#include <hm3/grid/hierarchical/types.hpp>
#include <hm3/types.hpp>

namespace hm3::solver {

/// Level-set solver
namespace level_set {

using hierarchical::grid_idx;
using hierarchical::grid_node_idx;
using hierarchical::tree_node_idx;

template <dim_t Ad>
struct state;

template <dim_t Ad>
using grid_client_t = ::hm3::grid::hierarchical::client::multi<Ad>;

}  // namespace hm3::solver::level_set
