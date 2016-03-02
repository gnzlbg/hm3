#pragma once
/// \file
///
/// Single hierarchical Cartesian grid
#include <hm3/grid/hierarchical/cartesian/node.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/node_level.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/node_neighbors.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/normalized_coordinates.hpp>
#include <hm3/grid/hierarchical/tree/tree.hpp>

namespace hm3 {
namespace grid {

//// Hierarchical Cartesian grids
namespace hc {

/// Hierarchical Cartesian grid
template <uint_t Nd>  //
struct single : tree::tree<Nd> {
  using tree_t                  = tree::tree<Nd>;
  using node_geometry_t         = geometry::square<Nd>;
  using point_t                 = geometry::point<Nd>;
  using node_t                  = node<Nd>;
  node_geometry_t bounding_box_ = {point_t::constant(0.5), 1.};

  using tree_t::dimension;
  using tree_t::dimensions;

  single()              = default;
  single(single const&) = default;
  single(single&&)      = default;
  single& operator=(single const&) = default;
  single& operator=(single&&) = default;

  single(tree_node_idx node_capacity, node_geometry_t bounding_box)
   : tree_t(std::move(node_capacity)), bounding_box_(std::move(bounding_box)) {}
  single(node_geometry_t bounding_box, tree_t tree_)
   : tree_t(std::move(tree_)), bounding_box_(std::move(bounding_box)) {}

  /// Bounding box of the grid (root node geometry)
  auto bounding_box() const noexcept { return bounding_box_; }

 protected:
  template <typename At>
  void assert_node_in_bounds(tree_node_idx n, At&& at) const noexcept {
    HM3_ASSERT_AT(n && n < tree_t::capacity(),
                  "node {} out-of-capacity-bounds [0, {})", at, n,
                  tree_t::capacity());
  }

  template <typename At>
  void assert_node_in_use(tree_node_idx n, At&& at) const noexcept {
    assert_node_in_bounds(n, at);
    HM3_ASSERT_AT(!tree_t::is_free(n), "node {} is not in use", at, n);
  }

 public:
  /// Length of node at level \p l
  num_t length(level_idx l) const noexcept {
    return geometry::length(bounding_box()) * node_length_at_level(l);
  }

  /// Length of node \p n
  num_t length(tree_node_idx n) const noexcept {
    assert_node_in_use(n, HM3_AT_);
    return length(level(n));
  }

  /// Center coordinates of node \p n
  point_t coordinates(tree_node_idx n) const noexcept {
    assert_node_in_use(n, HM3_AT_);
    auto xs = tree::normalized_coordinates(*this, n);
    return point_t{geometry::length(bounding_box()) * xs()};
  }

  /// Node at index \p n
  node_t node(tree_node_idx n) const noexcept {
    assert_node_in_use(n, HM3_AT_);
    return node_t{coordinates(n), length(n), n};
  }

  /// Geometry of node \p n
  geometry::square<Nd> geometry(tree_node_idx n) const noexcept {
    return node(n);
  }

  /// Level of node \p n
  level_idx level(tree_node_idx n) const noexcept {
    assert_node_in_use(n, HM3_AT_);
    return tree::node_level(*this, n);
  }

  /// All neighbors (across all manifolds) of node \p n
  inline auto neighbors(tree_node_idx n) const noexcept {
    assert_node_in_use(n, HM3_AT_);
    return tree::node_neighbors(*this, tree::node_location(*this, n));
  }

  /// All neighbors across \p manifold of node \p n
  template <typename Manifold>
  inline auto neighbors(tree_node_idx n, Manifold manifold) const noexcept {
    assert_node_in_use(n, HM3_AT_);
    return tree::node_neighbors(manifold, *this, tree::node_location(*this, n));
  }

  /// Center coordinates of neighbor \p p of node \p n
  ///
  /// Returns the correct coordinates even if:
  /// - neighbor does not exist
  /// - neighbor would lie outside of the bounding box
  ///
  template <typename NeighborPos, typename Manifold = get_tag_t<NeighborPos>>
  point_t coordinates(tree_node_idx n, NeighborPos p) const noexcept {
    assert_node_in_use(n, HM3_AT_);
    auto x_n          = coordinates(n);
    const auto delta  = length(n);
    const auto offset = Manifold()[p];
    for (auto&& d : dimensions()) { x_n += delta * offset[d]; }
    return x_n;
  }
};

template <uint_t Nd>
bool operator==(single<Nd> const& a, single<Nd> const& b) noexcept {
  using tree_t = tree::tree<Nd> const&;
  return a.bounding_box() == b.bounding_box()
         && static_cast<tree_t>(a) == static_cast<tree_t>(b);
}

template <uint_t Nd>
bool operator!=(single<Nd> const& a, single<Nd> const& b) noexcept {
  return !(a == b);
}

template <uint_t Nd> string type(single<Nd> const&) {
  return "hierarchical_cartesian_grid";
}

template <uint_t Nd> string name(single<Nd> const&) {
  return type(single<Nd>{}) + "_" + std::to_string(Nd) + "D";
}

}  // namespace hc
}  // namespace grid
}  // namespace hm3
