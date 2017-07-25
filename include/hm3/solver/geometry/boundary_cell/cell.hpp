#pragma once
/// \file
///
/// Boundary cell geometry.

#include <hm3/solver/geometry/boundary_cell/surface.hpp>
#include <hm3/solver/geometry/types.hpp>

namespace hm3::solver::geometry {

namespace boundary_cell {

/// Boundary cell data
struct data {
  cell_idx_t cell_idx;
};

// clang-format off

/// Geometry of a boundary cell
template <dim_t Ad, typename CellData>
using geometry
= std::conditional_t<Ad == 1, hg::segment<1, surface<1>, CellData>,
  std::conditional_t<Ad == 2, hg::polygon<2, surface<2>, CellData>,
  std::conditional_t<Ad == 3, hg::polyhedron<surface<3>, CellData>,
                     void>>>;
// clang-format on

// template <dim_t Ad, typename InternalSurface,  //
//           typename CellData = data>
// struct cell : geometry<Ad, SurfaceData, CellData> {
//   using base_t = geometry<Ad, SurfaceData, CellData>;
//   using base_t::base_t;
//   using base_t::operator=;
// };

}  // namespace boundary_cell

}  // namespace hm3::solver::geometry
