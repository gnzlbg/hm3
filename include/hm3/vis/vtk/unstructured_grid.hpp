#pragma once
/// \file
///
/// Unstructured VTK grid
#ifdef HM3_ENABLE_VTK
#include <hm3/vis/vtk/vtk.hpp>
#include <hm3/utility/log.hpp>

namespace hm3 {
namespace vis {
namespace vtk {

struct unstructured_grid {
  smart_ptr<vtkUnstructuredGrid> vtk_grid;
  hm3::log::serial log;

  /// Appends cells in range \p nodes to vtk grid
  ///
  /// Grid has to provide the following:
  /// - dimensions() returns a range of spatial dimensions
  /// - bounding_box() returns a bounding box of the grid
  /// - geometry(value_type<NodeRange>) function that returns the
  ///   geometry of a cell
  template <typename NodeRange, typename Grid,
            CONCEPT_REQUIRES_(InputRange<NodeRange>{})>
  void append_internal_cells(NodeRange&& nodes, Grid&& grid) noexcept {
    using grid_cell_t = decltype(grid.geometry(*begin(nodes)));
    using vtk_cell_t = to_vtk_cell_t<grid_cell_t>;

    const auto no_cells = distance(use_copy_if_single_pass(nodes));

    // Preallocate cell array:
    log("Allocating \"{}\" cells...", no_cells);
    vtkSmartPointer<vtkCellArray> cells = make_ptr<vtkCellArray>();
    cells->Allocate(no_cells);

    // Preallocate point array (we use a "vtkPointSet"):
    //  - N_vertex estimated as ~1.30 N_cells to avoid reallocation
    std::size_t approx_no_unique_vertices = no_cells * 1.30;
    log("Allocating \"{}\" vertices...", approx_no_unique_vertices);
    auto points = make_ptr<vtkPoints>();
    points->Allocate(approx_no_unique_vertices);

    /// Create a unique point inserter
    auto unique_inserter = make_ptr<vtkMergePoints>();
    auto bounds = geometry::bounds(grid.bounding_box());
    num_t bounds_vtk[6] = {0.0};  // vtk works only in 3D...
    for (auto d : grid.dimensions()) {
      bounds_vtk[d]     = bounds.min(d);
      bounds_vtk[d + 3] = bounds.max(d);
    }
    unique_inserter->InitPointInsertion(points, bounds_vtk);

    /// Create a temporary cell:
    log("Generating vtk grid...");
    auto tmp_cell = make_ptr<vtk_cell_t>();
    int_t c_c     = 0;
    RANGES_FOR (auto&& n, use_copy_if_single_pass(nodes)) {
      auto cell_geometry = grid.geometry(n);
      auto cell_corners  = geometry::corners(cell_geometry);
      // Append corners to the point set, and store the corner ids:
      RANGES_FOR (auto&& cIdx, grid_cell_t::corner_positions()) {
        vtkIdType id;
        auto corner_vtk = to_vtk_point(cell_corners[cIdx]);
        unique_inserter->InsertUniquePoint(corner_vtk.data(), id);
        tmp_cell->GetPointIds()->SetId(cIdx, id);
      }
      // Insert the cell:
      cells->InsertNextCell(tmp_cell->GetPointIds());
      ++c_c;
    }
    HM3_ASSERT(c_c == no_cells,
               "size mismatch: {} cells allocated but {} cells inserted",
               no_cells, c_c);

    vtk_grid->SetPoints(points);
    vtk_grid->SetCells(tmp_cell->GetCellType(), cells);
    log("vtk grid has \"{}\" cells (allocated: {}) and \"{}\" vertices "
        "(allocated: {})",
        cells->GetNumberOfCells(), no_cells, points->GetNumberOfPoints(),
        approx_no_unique_vertices);
  }

  template <typename Nodes, typename Grid>
  void reinitialize(Nodes&& nodes, Grid&& grid) {
    log("Reinitializing vtk grid...");
    // Create a vtk unstructured grid:
    vtk_grid = make_ptr<vtkUnstructuredGrid>();

    vtk_grid->Initialize();

    log("Appending cells to vtk grid...");
    append_internal_cells(std::forward<Nodes>(nodes), std::forward<Grid>(grid));

    log("Squeezing grid...");
    vtk_grid->Squeeze();

    log("...vtk grid reinitialized!");
  }
};

}  // namespace vtk
}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
