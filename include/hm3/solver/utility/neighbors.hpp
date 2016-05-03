#pragma once
/// \file
///
/// Stores neighbors of solver grid cells
#include <hm3/grid/hierarchical/client/neighbor_storage.hpp>

namespace hm3 {
namespace solver {
namespace state {

template <suint_t neighbor_capacity>
using neighbors
 = ::hm3::grid::hierarchical::client::neighbor_storage<neighbor_capacity>;

}  // namespace state
}  // namespace solver
}  // namespace hm3
