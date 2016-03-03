#pragma once
/// \file
///
/// Solver types
#include <hm3/grid/hierarchical/types.hpp>

namespace hm3 {
namespace solver {

using hierarchical::tree_node_idx;
using hierarchical::grid_node_idx;
using hierarchical::grid_idx;
using hierarchical::operator""_gn;
using hierarchical::operator""_g;

using tree::level_idx;

/// Index of a cell
using cell_idx
 = compact_optional<empty_scalar_value<idx_t,
                                       std::numeric_limits<idx_t>::max()>,
                    struct cell_idx_tag>;

constexpr cell_idx operator"" _c(unsigned long long int i) {
  return cell_idx{static_cast<idx_t>(i)};
}

/// Index of a variable
using var_idx
 = compact_optional<empty_scalar_value<sidx_t,
                                       std::numeric_limits<sidx_t>::max()>,
                    struct var_idx_tag>;

constexpr var_idx operator"" _v(unsigned long long int i) {
  return var_idx{static_cast<sidx_t>(i)};
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
