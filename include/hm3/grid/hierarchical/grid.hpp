#pragma once
/// \file
///
/// HM3's grid type
#include <hm3/grid/hierarchical/cartesian/multi.hpp>
#include <hm3/grid/hierarchical/types.hpp>

namespace hm3 {
namespace grid {

/// Single hierarchical Cartesian Grid
template <uint_t Nd> using shc = hc::single<Nd>;

/// Multi hierarchical Cartesian Grid
template <uint_t Nd> using mhc = hc::multi<Nd>;

}  // namespace grid
}  // namespace hm3
