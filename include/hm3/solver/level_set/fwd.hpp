#pragma once
/// \file fwd.hpp
#include <hm3/types.hpp>
#include <hm3/grid/types.hpp>

namespace hm3 {
namespace solver {

/// Level-set solver
namespace level_set {

using grid::grid_idx;
using grid::grid_node_idx;
using grid::tree_node_idx;

template <uint_t nd> struct state;

}  // namespace level_set
}  // namespace solver
}  // namespace hm3
