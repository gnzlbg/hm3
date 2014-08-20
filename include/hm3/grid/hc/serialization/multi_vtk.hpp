#pragma once
/// \file
///
/// Serialization of grid::hc::multi to VKT
#ifdef HM3_ENABLE_VTK
#include <hm3/grid/hc/multi.hpp>
#include <hm3/tree/algorithm/node_level.hpp>
#include <hm3/tree/algorithm/node_neighbor.hpp>
#include <hm3/geometry/square.hpp>
#include <hm3/utility/log.hpp>
#include <hm3/vis/vtk/unstructured_grid.hpp>
#include <hm3/vis/vtk/cell_data.hpp>

namespace hm3 {
namespace grid {
namespace hc {
namespace vtk {

/// Adapts a tree<Nd> to be Serializable to a vtkUnstructuredGrid
///  - a cell_geometry(node_idx) method
///  - a bounding_box() method
///  - a dimensions() method
///  - a for_each_cell(F&&) method
template <uint_t Nd> struct serializable_multi {
  multi<Nd> const& t_;
  int_t level_ = -1;

  using cell_data_idx = tree_node_idx;

  serializable_multi(multi<Nd> const& s, int_t level = -1) noexcept
   : t_{s},
     level_{level} {}

  auto geometry(tree_node_idx n) const noexcept { return t_.geometry(n); }

  auto nodes() const noexcept {
    return t_.nodes() | view::filter([&](tree_node_idx n) {
             return (level_ < 0) ? t_.is_leaf(n)
                                 : t_.level(n) == static_cast<uint_t>(level_);
           });
  }

  auto bounding_box() const noexcept { return t_.bounding_box(); }

  static auto dimensions() noexcept { return hm3::dimensions(Nd); }

  template <typename F> auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }
};

template <uint_t Nd>
void serialize(multi<Nd> const& t, string const& fname_, int_t level = -1) {
  using serializable = serializable_multi<Nd>;
  string fname       = fname_ + ".vtu";
  hm3::log::serial log("tree<Nd>-vtk-dump");

  serializable state(t, level);

  vis::vtk::unstructured_grid vtk_grid;
  vtk_grid.log = log;

  vtk_grid.reinitialize(state.nodes(), state);

  using cell_data_t
   = vis::vtk::cell_data<serializable, vis::vtk::unstructured_grid>;
  cell_data_t cell_data(state, vtk_grid, log);

  cell_data.load("idx", [&](tree_node_idx n, auto&&) {
    return n ? static_cast<int_t>(*n) : int_t{-1};
  });
  cell_data.load("parent", [&](tree_node_idx n, auto&&) {
    auto p = t.parent(n);
    return p ? static_cast<int_t>(*p) : int_t{-1};
  });

  cell_data.load("level", [&](tree_node_idx n, auto&&) { return *t.level(n); });

  for (auto gidx : t.grids()) {
    cell_data.load("grid_" + std::to_string(*gidx),
                   [&](tree_node_idx n, auto&&) {
                     auto p = t.node(n, gidx);
                     return p ? static_cast<int_t>(*p) : int_t{-1};
                   });
  }

  vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer
   = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
  writer->SetFileName(fname.c_str());
  writer->SetInputData(vtk_grid.vtk_grid);
  writer->Write();
}

}  // namespace vtk
}  // namespace hc
}  // namespace grid
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
