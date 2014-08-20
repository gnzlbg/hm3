#pragma once
/// \file grid.hpp HM3's grid

#include <hm3/grid/types.hpp>
#include <hm3/grid/hc/multi.hpp>

namespace hm3 {
namespace grid {

/// Multi hierarchical Cartesian Grid
template <uint_t nd> using mhc = hc::multi<nd>;

}  // namespace grid
}  // namespace hm3
