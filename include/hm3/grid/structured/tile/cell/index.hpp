#pragma once
/// \file
///
/// Square tile cell index
#include <hm3/grid/structured/tile/cell/bounds.hpp>
#include <hm3/grid/structured/tile/index_type.hpp>
#include <hm3/utility/bounded.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace tile {
namespace cell {

/// Square tile cell index
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc number of cells per tile length
template <suint_t Nd, suint_t Nc>  //
using index = bounded<index_type, index_type{0}, bounds<Nd, Nc>::size(),
                      struct tile_cell_index_tag>;

}  // namespace cell
}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
