#pragma once
/// \file
///
/// Grid node type
#include <hm3/geometry/box.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/grid/hierarchical/types.hpp>

namespace hm3 {
namespace grid {
namespace hierarchical {
namespace cartesian {

/// Hierarchical Cartesian grid node
template <dim_t Nd>  //
struct node : geometry::box<Nd>, tree_node_idx {
  using point_t               = geometry::point<Nd>;
  constexpr node()            = default;
  constexpr node(node const&) = default;
  constexpr node(node&&)      = default;
  constexpr node& operator=(node const&) = default;
  constexpr node& operator=(node&&) = default;

  /// Node from \p x_min and \p x_max.
  constexpr node(point_t x_min, point_t x_max,
                 tree_node_idx i = tree_node_idx{})
   : geometry::box<Nd>{x_min, x_max}, tree_node_idx{i} {}
  /// Node from \p x_center and \p x_length.
  constexpr node(point_t x_center, num_t length,
                 tree_node_idx i = tree_node_idx{})
   : geometry::box<Nd>{x_center, length}, tree_node_idx{i} {}
  /// Node from \p box and index \p i
  constexpr node(geometry::box<Nd> box, tree_node_idx i = tree_node_idx{})
   : geometry::box<Nd>{box}, tree_node_idx{i} {}
};

template <dim_t Nd>
constexpr bool operator==(node<Nd> const& a, node<Nd> const& b) noexcept {
  return static_cast<tree_node_idx>(a) == static_cast<tree_node_idx>(b)
         and static_cast<geometry::box<Nd>>(a)
              == static_cast<geometry::box<Nd>>(b);
}

template <dim_t Nd>
constexpr bool operator!=(node<Nd> const& a, node<Nd> const& b) noexcept {
  return !(a == b);
}

}  // namespace cartesian
}  // namespace hierarchical
}  // namespace grid
}  // namespace hm3
