#pragma once
/// \file
///
/// Tree serialization to VKT
#ifdef HM3_ENABLE_VTK
#include <hm3/geometry/fwd.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/node_level.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/node_neighbor.hpp>
#include <hm3/grid/hierarchical/tree/tree.hpp>
#include <hm3/utility/log.hpp>
#include <hm3/vis/vtk/serialize.hpp>

namespace hm3::tree {

namespace vtk {

/// Adapts a tree<Ad> to be Serializable to a vtkUnstructuredGrid
///  - a cell_geometry(node_idx) method
///  - a bounding_box() method
///  - a ambient_dimensions() method
///  - a for_each_cell(F&&) method
template <dim_t Ad>
struct serializable_tree : geometry::with_ambient_dimension<Ad> {
  tree<Ad> const& t_;
  sint_t level_ = -1;

  using vtk_cell_idx = node_idx;

  serializable_tree(tree<Ad> const& s, sint_t level = -1) noexcept
   : t_{s}, level_{level} {}

  auto geometry(node_idx n) const noexcept {
    auto loc = node_location(t_, n);
    auto x_c = normalized_coordinates(loc);
    return geometry::box<Ad>(x_c, node_length_at_level(loc.level()));
  }

  auto nodes() const noexcept {
    return t_.nodes() | view::filter([&](node_idx n) {
             return (level_ < 0)
                     ? t_.is_leaf(n)
                     : node_level(t_, n) == static_cast<lidx_t>(level_);
           });
  }

  static auto bounding_box() noexcept {
    return geometry::box<Ad>(geometry::point<Ad>::constant(0.5), 1.0);
  }

  template <typename F>
  auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }

  template <typename CellData>
  void load(CellData&& cell_data) const {
    cell_data.load("idx", [&](node_idx n, auto&&) {
      return n ? static_cast<int_t>(*n) : int_t{-1};
    });
    cell_data.load("parent", [&](auto&& n, auto&&) {
      auto p = t_.parent(n);
      return p ? static_cast<int_t>(*p) : int_t{-1};
    });

    cell_data.load("level",
                   [&](auto&& n, auto&&) { return *node_level(t_, n); });

    if (Ad > 0) {
      auto ns = manifold_neighbors<Ad, 1>{};
      cell_data.load("face_neighbors",
                     [&](auto&& n, auto&& c) {
                       auto nghbr = node_neighbor(t_, n, ns.idx(c));
                       return nghbr ? static_cast<int_t>(*nghbr) : int_t{-1};
                     },
                     ns.size());
    }
    if (Ad > 1) {
      auto ns = manifold_neighbors<Ad, 2>{};
      cell_data.load("edge_neighbors",
                     [&](auto&& n, auto&& c) {
                       auto nghbr = node_neighbor(t_, n, ns.idx(c));
                       return nghbr ? static_cast<int_t>(*nghbr) : int_t{-1};
                     },
                     ns.size());
    }
    if (Ad > 2) {
      auto ns = manifold_neighbors<Ad, 3>{};
      cell_data.load("corner_neighbors",
                     [&](auto&& n, auto&& c) {
                       auto nghbr = node_neighbor(t_, n, ns.idx(c));
                       return nghbr ? static_cast<int_t>(*nghbr) : int_t{-1};
                     },
                     ns.size());
    }
  }
};

template <dim_t Ad>
void serialize(tree<Ad> const& t, string const& fname_, sint_t level = -1) {
  using serializable = serializable_tree<Ad>;
  string fname       = fname_ + ".vtu";
  hm3::log::serial log("tree<Ad>-vtk-dump");

  serializable state(t, level);
  ::hm3::vis::vtk::serialize(state, fname_, log);
}

}  // namespace vtk
}  // namespace hm3::tree
#endif  // HM3_ENABLE_VTK
