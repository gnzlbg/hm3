#pragma once
/// \file
///
/// HM3's grid type
#include <hm3/grid/hierarchical/cartesian/multi.hpp>
#include <hm3/grid/hierarchical/types.hpp>

namespace hm3::grid {

/// Hierarchical grids data-structures and algorithms
namespace hierarchical {

/// Single hierarchical Cartesian Grid
template <dim_t Ad>
using cs = cartesian::single<Ad>;

/// Multi hierarchical Cartesian Grid
template <dim_t Ad>
using cm = cartesian::multi<Ad>;

}  // namespace hierarchical

using namespace grid;

}  // namespace hm3::grid
