#pragma once
/// \file
///
/// Square structured grid
#include <hm3/grid/structured/halo_tile/tile.hpp>
#include <hm3/grid/structured/tile/tile.hpp>

namespace hm3::grid {

/// Square structured grid algorithms
namespace structured {

using tidx_t = tile::tidx_t;

template <dim_t Ad, tidx_t Nic, tidx_t Nhl>
using grid = halo_tile::tile<Ad, Nic, Nhl>;

}  // namespace structured

}  // namespace hm3::grid
