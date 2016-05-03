#pragma once
/// \file
///
///
/// Stores a solver grid in sync with the tree
#include <hm3/grid/hierarchical/client/multi.hpp>
#include <hm3/grid/hierarchical/grid.hpp>
#include <hm3/solver/types.hpp>

namespace hm3 {
namespace solver {
namespace state {

template <dim_t Nd> using grid = grid::hierarchical::client::multi<Nd>;

}  // namespace state
}  // namespace solver
}  // namespace hm3
