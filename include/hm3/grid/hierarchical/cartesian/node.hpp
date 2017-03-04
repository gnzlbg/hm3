#pragma once
/// \file
///
/// Grid node type
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/grid/hierarchical/types.hpp>

namespace hm3::grid::hierarchical::cartesian {

/// Hierarchical Cartesian grid node
template <dim_t Nd>
struct node : geometry::box<Nd>, tree_node_idx {
  using point_t               = geometry::point<Nd>;
  using box_t                 = geometry::box<Nd>;
  constexpr node()            = default;
  constexpr node(node const&) = default;
  constexpr node(node&&)      = default;
  constexpr node& operator=(node const&) = default;
  constexpr node& operator=(node&&) = default;

  /// Node from \p x_min and \p x_max.
  constexpr node(point_t x_min, point_t x_max,
                 tree_node_idx i = tree_node_idx{})
   : box_t{x_min, x_max}, tree_node_idx{i} {}
  /// Node from \p x_center and \p x_length.
  constexpr node(point_t x_center, num_t length,
                 tree_node_idx i = tree_node_idx{})
   : box_t{x_center, length}, tree_node_idx{i} {}
  /// Node from \p box and index \p i
  constexpr node(box_t box, tree_node_idx i = tree_node_idx{})
   : box_t{box}, tree_node_idx{i} {}
};

template <dim_t Nd>
constexpr bool operator==(node<Nd> const& a, node<Nd> const& b) noexcept {
  return static_cast<tree_node_idx const&>(a)
          == static_cast<tree_node_idx const&>(b)
         and static_cast<geometry::box<Nd> const&>(a)
              == static_cast<geometry::box<Nd> const&>(b);
}

template <dim_t Nd>
constexpr bool operator!=(node<Nd> const& a, node<Nd> const& b) noexcept {
  return !(a == b);
}

}  // namespace hm3::grid::hierarchical::cartesian
