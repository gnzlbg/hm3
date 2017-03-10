#ifdef FIXED
#pragma once
/// \file
///
/// FV cell types.

namespace hm3::solver::fv {

/// Types of tile cells.
enum class tile_cell_type : char {
  internal                        = 0,
  boundary                        = 1,
  internal_boundary_cell_neighbor = 2,
  external                        = 3,
  unknown                         = 4
};

}  // namespace hm3::solver::fv
#endif
