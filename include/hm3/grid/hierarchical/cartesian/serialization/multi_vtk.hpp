#pragma once
/// \file
///
/// Serialization of grid::hierarchical::cartesian::multi to VKT
#ifdef HM3_ENABLE_VTK
#include <hm3/geometry/square.hpp>
#include <hm3/grid/hierarchical/cartesian/multi.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/node_level.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/node_neighbor.hpp>
#include <hm3/utility/log.hpp>
#include <hm3/vis/vtk/geometry.hpp>
#include <hm3/vis/vtk/serialize.hpp>

namespace hm3 {
namespace grid {
namespace hierarchical {
namespace cartesian {

/// Serialization to VTK
namespace vtk {

/// Adapts a tree<Nd> to be Serializable to a vtkUnstructuredGrid
///  - a cell_geometry(node_idx) method
///  - a bounding_box() method
///  - a dimensions() method
///  - a for_each_cell(F&&) method
template <dim_t Nd> struct serializable_multi : geometry::dimensional<Nd> {
  multi<Nd> const& t_;
  sint_t level_ = -1;

  using vtk_cell_idx = tree_node_idx;

  serializable_multi(multi<Nd> const& s, sint_t level = -1) noexcept
   : t_{s}, level_{level} {}

  auto geometry(tree_node_idx n) const noexcept { return t_.geometry(n); }

  auto nodes() const noexcept {
    return t_.nodes() | view::filter([&](tree_node_idx n) {
             return (level_ < 0)
                     ? t_.is_leaf(n)
                     : t_.level(n) == static_cast<tree::lidx_t>(level_);
           });
  }

  auto bounding_box() const noexcept { return t_.bounding_box(); }

  template <typename F> auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }

  template <typename CellData> void load(CellData&& cell_data) const noexcept {
    cell_data.load("idx", [&](tree_node_idx n, auto&&) {
      return n ? static_cast<int_t>(*n) : int_t{-1};
    });
    cell_data.load("parent", [&](tree_node_idx n, auto&&) {
      auto p = t_.parent(n);
      return p ? static_cast<int_t>(*p) : int_t{-1};
    });

    cell_data.load("level",
                   [&](tree_node_idx n, auto&&) { return *t_.level(n); });

    for (auto gidx : t_.grids()) {
      cell_data.load("grid_" + std::to_string(*gidx),
                     [&](tree_node_idx n, auto&&) {
                       auto p = t_.node(n, gidx);
                       return p ? static_cast<int_t>(*p) : int_t{-1};
                     });
    }
  }
};

template <dim_t Nd>
void serialize(multi<Nd> const& t, string const& fname_, sint_t level = -1) {
  using serializable = serializable_multi<Nd>;
  hm3::log::serial log("multi-tree<Nd>-vtk-dump");

  serializable state(t, level);
  ::hm3::vis::vtk::serialize(state, fname_, log);
}

}  // namespace vtk
}  // namespace cartesian
}  // namespace hierarchical
}  // namespace grid
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
