#pragma once
/// \file node.hpp Grid node
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/square.hpp>
#include <hm3/grid/types.hpp>

namespace hm3 {
namespace grid {
namespace hc {

/// Hierarchical Cartesian grid node
template <uint_t nd>  //
struct node : geometry::square<nd>, tree_node_idx {
  using point_t = geometry::point<nd>;
  constexpr node() = default;
  constexpr node(node const&) = default;
  constexpr node(node&&)  = default;
  constexpr node& operator=(node const&) = default;
  constexpr node& operator=(node&&) = default;

  /// Node from \p x_min and \p x_max.
  constexpr node(point_t x_min, point_t x_max,
                 tree_node_idx i = tree_node_idx{})
   : geometry::square<nd>{x_min, x_max}, tree_node_idx{i} {}
  /// Node from \p x_center and \p x_length.
  constexpr node(point_t x_center, num_t length,
                 tree_node_idx i = tree_node_idx{})
   : geometry::square<nd>{x_center, length}, tree_node_idx{i} {}
  /// Node from \p square and index \p i
  constexpr node(geometry::square<nd> square, tree_node_idx i = tree_node_idx{})
   : geometry::square<nd>{square}, tree_node_idx{i} {}
};

template <uint_t nd>
constexpr bool operator==(node<nd> const& a, node<nd> const& b) noexcept {
  return static_cast<tree_node_idx>(a) == static_cast<tree_node_idx>(b)
         and static_cast<geometry::square<nd>>(a)
              == static_cast<geometry::square<nd>>(b);
}

template <uint_t nd>
constexpr bool operator!=(node<nd> const& a, node<nd> const& b) noexcept {
  return !(a == b);
}

}  // namespace hc
}  // namespace grid
}  // namespace hm3
