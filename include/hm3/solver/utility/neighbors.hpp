#pragma once
/// \file
///
/// Stores neighbors of solver grid cells
#include <hm3/solver/state/grid.hpp>
#include <hm3/utility/stack_vector.hpp>
#include <vector>

namespace hm3 {
namespace solver {
namespace state {

template <uint_t neighbor_capacity>  //
struct neighbors {
  using neighbor_vector = stack::vector<grid_node_idx, neighbor_capacity>;

  std::vector<neighbor_vector> data_;

  neighbors(grid_node_idx capacity_) : data_(*capacity_) {}

  template <typename At>
  void assert_within_capacity(grid_node_idx n, At&& at) const noexcept {
    HM3_ASSERT_AT(n, "invalid node", at);
    HM3_ASSERT_AT(static_cast<std::size_t>(*n) < data_.capacity(),
                  "node {} is out-of-capacity-bounds [0, {})", at, n,
                  data_.capacity());
  }

  /// Does node \p n have node \p neighbor as neighbor?
  auto has_neighbor(grid_node_idx node, grid_node_idx neighbor) const noexcept {
    assert_within_capacity(node, HM3_AT_);
    assert_within_capacity(neighbor, HM3_AT_);
    HM3_ASSERT(neighbor, "{}", node);
    auto it = find(data_[*node], neighbor);
    HM3_ASSERT(
     it == end(data_[*node])
      || (find(data_[*neighbor], node) != end(data_[*neighbor])),
     "if node {} has {} as neighbor, {} must have {} as neighbor as well", node,
     neighbor, neighbor, node);
    return it != end(data_[*node]) ? true : false;
  }

  /// Adds neighbor \p neighbor to node \p node (and viceversa)
  void push(grid_node_idx node, grid_node_idx neighbor) {
    assert_within_capacity(node, HM3_AT_);
    assert_within_capacity(neighbor, HM3_AT_);
    HM3_ASSERT(data_[*node].size() != neighbor_capacity, "{}, {}", node,
               neighbor);
    HM3_ASSERT(data_[*neighbor].size() != neighbor_capacity, "{}, {}", node,
               neighbor);
    HM3_ASSERT(!has_neighbor(node, neighbor),
               "node {} already has {} as neighbor", node, neighbor);
    HM3_ASSERT(!has_neighbor(neighbor, node),
               "neighbor {} already has node {} as neighbor", neighbor, node);

    data_[*node].push_back(neighbor);
    data_[*neighbor].push_back(node);
  }

  template <typename NeighborRange,
            CONCEPT_REQUIRES_(InputRange<NeighborRange>{})>
  void push(grid_node_idx node, NeighborRange&& grid_neighbors) {
    for (auto n : grid_neighbors) { push(node, n); }
  }

  /// Remove neighbor \p neighbor from node \p node
  void pop(grid_node_idx node, grid_node_idx neighbor) {
    assert_within_capacity(node, HM3_AT_);
    assert_within_capacity(neighbor, HM3_AT_);
    HM3_ASSERT(has_neighbor(node, neighbor),
               "node {} does not have {} as neighbor", node, neighbor);
    HM3_ASSERT(has_neighbor(neighbor, node),
               "neighobr {} does not have node {} as neighbor", neighbor, node);
    action::remove_if(data_[*node],
                      [&](grid_node_idx n) { return n == neighbor; });
    action::remove_if(data_[*neighbor],
                      [&](grid_node_idx n) { return n == node; });
    HM3_ASSERT(!has_neighbor(node, neighbor), "node {} still has neighbor {}",
               node, neighbor);
    HM3_ASSERT(!has_neighbor(neighbor, node), "neighbor {} still has node {}",
               neighbor, node);
  }

  void pop(grid_node_idx node) {
    auto ns = (*this)(node);  // make copy
    for (auto n : ns) { pop(node, n); }
    HM3_ASSERT(data_[*node].empty(), "removing neighbors for node {} failed!",
               node);
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
    auto update_self_neighbors = [&](auto&& ns, auto&& from, auto&& to) {
      for (auto&& n : ns) {
        if (n == to) { n = from; }
      }
    };

    auto&& ins = data_[*i];
    auto&& jns = data_[*j];

    update_neighbors(ins, i, j);
    update_neighbors(jns, j, i);

    if (has_neighbor(i, j)) {
      update_self_neighbors(ins, i, j);
      update_self_neighbors(jns, j, i);
    }

    ranges::swap(ins, jns);
  }

  neighbor_vector const& operator()(grid_node_idx node) const noexcept {
    assert_within_capacity(node, HM3_AT_);
    return data_[*node];
  }
};

}  // namespace state
}  // namespace solver
}  // namespace hm3
