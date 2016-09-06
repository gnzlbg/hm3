#pragma once
/// \file
///
/// Solver types
#include <hm3/grid/hierarchical/types.hpp>
#include <hm3/grid/structured/tile/index_type.hpp>

namespace hm3 {
namespace solver {

using hierarchical::tree_node_idx;
using hierarchical::grid_node_idx;
using hierarchical::grid_idx;
using hierarchical::operator""_gn;
using hierarchical::operator""_g;

using tree::level_idx;

using tidx_t = grid::structured::tile::tidx_t;

using cidx_t = idx_t;

/// Index of a cell
using cell_idx = optional_idx<cidx_t, struct cell_idx_tag>;

constexpr cell_idx operator"" _c(unsigned long long int i) {
  return cell_idx{static_cast<idx_t>(i)};
}

using vidx_t = suint_t;

/// Index of a variable
using var_idx = optional_idx<vidx_t, struct var_idx_tag>;

constexpr var_idx operator"" _v(unsigned long long int i) {
  return var_idx{static_cast<sidx_t>(i)};
}

/// Index of a surface
using surface_idx = optional_idx<idx_t, struct surface_idx_tag>;

constexpr surface_idx operator"" _sf(unsigned long long int i) {
  return surface_idx{static_cast<idx_t>(i)};
}

/// Index of boundary
using boundary_idx = optional_idx<sidx_t, struct boundary_idx_tag>;

constexpr boundary_idx operator"" _b(unsigned long long int i) {
  return boundary_idx{static_cast<sidx_t>(i)};
}

}  // namespace solver
}  // namespace hm3
