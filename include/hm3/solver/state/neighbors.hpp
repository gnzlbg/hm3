#pragma once
/// \file neighbors.hpp
#include <vector>
#include <hm3/solver/state/grid.hpp>
#include <hm3/utility/stack/vector.hpp>

namespace hm3 {
namespace solver {
namespace state {

template <uint_t neighbor_capacity>  //
struct neighbors {
  using neighbor_vector = stack::vector<grid_node_idx, neighbor_capacity>;

  std::vector<neighbor_vector> data_;

  neighbors(grid_node_idx capacity_) : data_{*capacity_} {}

  template <typename At>
  void assert_within_capacity(grid_node_idx n, At&& at) const noexcept {
    HM3_ASSERT_AT(n, "invalid node", at);
    HM3_ASSERT_AT(*n < data_.capacity(),
                  "node {} is out-of-capacity-bounds [0, {})", at, n,
                  data_.capacity());
  }

  /// Does node \p n have node \p neighbor as neighbor?
  auto has_neighbor(grid_node_idx node, grid_node_idx neighbor) const noexcept {
    assert_within_capacity(node, HM3_AT_);
    assert_within_capacity(neighbor, HM3_AT_);
    HM3_ASSERT(neighbor, "{}", node);
    auto it = find(data_[*node], neighbor);
    return it != end(data_[*node]) ? true : false;
  }

  /// Adds neighbor \p neighbor to node \p node
  void push(grid_node_idx node, grid_node_idx neighbor) {
    assert_within_capacity(node, HM3_AT_);
    assert_within_capacity(neighbor, HM3_AT_);
    HM3_ASSERT(data_[*node].size() != neighbor_capacity, "{}, {}", node,
               neighbor);
    data_[*node].push_back(neighbor);
  }

  /// Remove neighbor \p neighbor from node \p node
  void pop(grid_node_idx node, grid_node_idx neighbor) {
    assert_within_capacity(node, HM3_AT_);
    assert_within_capacity(neighbor, HM3_AT_);
    HM3_ASSERT(!data_[*node].empty(), "{} empty", node);
    HM3_ASSERT(has_neighbor(node, neighbor), "{}, {}", node, neighbor);
    auto&& ns = data_[*node];
    action::remove_if(data_[*node],
                      [&](grid_node_idx n) { return n == neighbor; });
  }

  void swap(grid_node_idx i, grid_node_idx j) noexcept {
    assert_within_capacity(i, HM3_AT_);
    assert_within_capacity(j, HM3_AT_);

    auto update_neighbors = [&](auto&& ns, auto&& from, auto&& to) {
      for (auto&& n : ns) {
        HM3_ASSERT(has_neighbor(n, from), "{}, {}", n, from);
        auto&& bns = data_[*n];
        for (auto&& bn : bns) {
          if (bn == from) {
            bn = to;
            break;
          }
        }
      }
    };

    auto&& ins = data_[*i];
    auto&& jns = data_[*j];

    update_neighbors(ins, i, j);
    update_neighbors(jns, j, i);
    ranges::swap(ins, jns);
  }
};

}  // namespace state
}  // namespace solver
}  // namespace hm3
