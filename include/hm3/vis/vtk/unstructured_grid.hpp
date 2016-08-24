#pragma once
/// \file
///
/// Unstructured VTK grid
#ifdef HM3_ENABLE_VTK
#include <hm3/utility/log.hpp>
#include <hm3/utility/variant.hpp>

#include <hm3/geometry/primitive/any.hpp>
#include <hm3/vis/vtk/vtk.hpp>

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
    using grid_t             = uncvref_t<Grid>;
    static constexpr auto nd = grid_t::dimension();
    auto geometry
     = [&](auto&& n) -> geometry::any<nd> { return grid.geometry(n); };

    using grid_cell_t   = decltype(geometry(*begin(nodes)));
    const auto no_cells = ranges::distance(use_copy_if_single_pass(nodes));

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
    auto bounds          = geometry::bounding_volume.aabb(grid.bounding_box());
    num_t bounds_vtk[6]  = {0.0};  // vtk works only in 3D...
    for (auto d : grid.dimensions()) {
      bounds_vtk[d]     = geometry::x_min(bounds)(d);
      bounds_vtk[d + 3] = geometry::x_max(bounds)(d);
    }
    unique_inserter->InitPointInsertion(points, bounds_vtk);

    /// Create a temporary cell:
    log("Generating vtk grid...");
    auto tmp_cells = make_tuple_of_cells(grid_cell_t{});
    vector<int> cell_types;
    cell_types.reserve(no_cells);
    int_t c_c = 0;
    RANGES_FOR (auto&& n, use_copy_if_single_pass(nodes)) {
      auto cell_geometry = geometry(n);
      visit(
       [&](auto&& g) {
         using geometry_t   = uncvref_t<decltype(g)>;
         auto cell_vertices = vertices(g);
         auto&& tmp_cell = std::get<to_vtk_cell_ptr_t<geometry_t>>(tmp_cells);
         HM3_ASSERT(size(cell_vertices) > 0_u,
                    "cells of zero points not supported (see comment below)");
         // TL;DR: Cells that don't have any points break too much stuff in too
         // many different places. Improving this is not worth it.
         //
         // ex1: the cell "exists" so we will try to compute and store cell
         //      variables for it but the cell has no center, possibly breaking
         //      computation, and they cannot be displayed anyways.
         //
         // ex2: the cell offsets will be an empty range [firstPointId,
         //      firstPointId), but they are stored in an offset array within
         //      ParaView which requires it to be monotonically increasing (e.g.
         //      the next value has to be greater than the previous one). Even
         //      if it is possible to work around this somehow, that's going to
         //      be painful.
         tmp_cell->GetPointIds()->Reset();
         // Append vertices to the point set, and store the corner ids:
         RANGES_FOR (auto&& cIdx, geometry::vertex_indices(g)) {
           vtkIdType id;
           auto corner_vtk = to_vtk_point(cell_vertices[cIdx]);
           unique_inserter->InsertUniquePoint(corner_vtk.data(), id);
           // TODO: this is faster, tmp_cell->GetPointIds()->SetId(cIdx, id);
           // so instead of just doing Reset above, we should get
           // size(vertices())
           // and initialize the cell points, so that we can use SetId
           //
           // The InsertId function does bound checks, so we might want to use
           // that in Debug mode anyways:
           tmp_cell->GetPointIds()->InsertId(cIdx, id);
         }
         // Insert the cell:
         cells->InsertNextCell(tmp_cell->GetPointIds());
         cell_types.push_back(cell_t<geometry_t>::value());
         ++c_c;
       },
       cell_geometry);
    }
    HM3_ASSERT(c_c == no_cells,
               "size mismatch: {} cells allocated but {} cells inserted",
               no_cells, c_c);

    vtk_grid->SetPoints(points);
    vtk_grid->SetCells(cell_types.data(), cells);
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
