#pragma once
/// \file
///
/// Square tile surface index
#include <hm3/grid/structured/tile/index_type.hpp>
#include <hm3/grid/structured/tile/surface/bounds.hpp>
#include <hm3/utility/bounded.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace tile {
namespace surface {

/// Square tile surface index
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc number of surfaces per tile length
template <dim_t Nd, tidx_t Nc>
using index = bounded<tidx_t, tidx_t{0}, bounds<Nd, Nc>::size(),
                      struct tile_surface_index_tag>;

}  // namespace surface
}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
