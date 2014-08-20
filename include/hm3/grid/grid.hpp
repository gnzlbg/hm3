#pragma once
/// \file
///
/// HM3's grid type
#include <hm3/grid/types.hpp>
#include <hm3/grid/hc/multi.hpp>

namespace hm3 {
namespace grid {

/// Multi hierarchical Cartesian Grid
template <uint_t Nd> using mhc = hc::multi<Nd>;

}  // namespace grid
}  // namespace hm3
