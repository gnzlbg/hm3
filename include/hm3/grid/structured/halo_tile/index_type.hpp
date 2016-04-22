#pragma once
/// \file
///
/// Type of the coordinate and tile indices
#include <hm3/grid/structured/tile/index_type.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace halo_tile {

/// Underlying type of the halo tile (c) and coordinate {i,j,k} indices
using tidx_t = tile::tidx_t;

}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
