#pragma once
/// \file
///
/// Square tile cell index
#include <hm3/grid/structured/tile/cell/bounds.hpp>
#include <hm3/grid/structured/tile/index_type.hpp>
#include <hm3/utility/bounded.hpp>

namespace hm3::grid::structured::tile::cell {

/// Square tile cell index
///
/// \tparam Ad number of spatial dimensions
/// \tparam Nc number of cells per tile length
template <dim_t Ad, tidx_t Nc>
using index = bounded<tidx_t, tidx_t{0}, bounds<Ad, Nc>::size(),
                      struct tile_cell_index_tag>;

}  // namespace hm3::grid::structured::tile::cell
