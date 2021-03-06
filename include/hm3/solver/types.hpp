#pragma once
/// \file
///
/// Solver types
#include <hm3/grid/types.hpp>

namespace hm3 {
namespace solver {

using grid::tree_node_idx;
using grid::grid_node_idx;
using grid::grid_idx;
using grid::operator""_gn;
using grid::operator""_g;

using tree::level_idx;

/// Index of a cell
using cell_idx
 = compact_optional<empty_scalar_value<idx_t,
                                       std::numeric_limits<idx_t>::max()>,
                    struct cell_idx_tag>;

constexpr cell_idx operator"" _c(unsigned long long int i) {
  return cell_idx{static_cast<idx_t>(i)};
}

/// Index of a surface
using surface_idx
 = compact_optional<empty_scalar_value<idx_t,
                                       std::numeric_limits<idx_t>::max()>,
                    struct surface_idx_tag>;

constexpr surface_idx operator"" _sf(unsigned long long int i) {
  return surface_idx{static_cast<idx_t>(i)};
}

/// Index of a boundary condition
using bc_idx
 = compact_optional<empty_scalar_value<idx_t,
                                       std::numeric_limits<idx_t>::max()>,
                    struct bc_idx_tag>;

constexpr bc_idx operator"" _bc(unsigned long long int i) {
  return bc_idx{static_cast<idx_t>(i)};
}

}  // namespace solver
}  // namespace hm3
