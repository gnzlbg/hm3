#pragma once
/// \file
///
/// HM3's grid type
#include <hm3/grid/hierarchical/cartesian/multi.hpp>
#include <hm3/grid/hierarchical/types.hpp>

namespace hm3 {
namespace grid {

/// Hierarchical grids data-structures and algorithms
namespace hierarchical {

/// Single hierarchical Cartesian Grid
template <dim_t Nd> using cs = cartesian::single<Nd>;

/// Multi hierarchical Cartesian Grid
template <dim_t Nd> using cm = cartesian::multi<Nd>;

}  // namespace hierarchical
}  // namespace grid

using namespace grid;

}  // namespace hm3
