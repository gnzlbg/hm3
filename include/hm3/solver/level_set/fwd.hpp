#pragma once
/// \file
///
/// Forward declarations of level-set solver
#include <hm3/grid/hierarchical/types.hpp>
#include <hm3/types.hpp>

namespace hm3 {
namespace solver {

/// Level-set solver
namespace level_set {

using hierarchical::grid_idx;
using hierarchical::grid_node_idx;
using hierarchical::tree_node_idx;

template <uint_t Nd> struct state;

}  // namespace level_set
}  // namespace solver
}  // namespace hm3
