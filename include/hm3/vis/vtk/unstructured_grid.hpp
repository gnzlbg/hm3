#pragma once
/// \file
///
/// Unstructured VTK grid
#ifdef HM3_ENABLE_VTK
#include <hm3/utility/log.hpp>
#include <hm3/vis/vtk/vtk.hpp>

namespace hm3::vis::vtk {

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
    static constexpr auto ad = geometry::ambient_dimension_v<grid_t>;
    using geometry::ambient_dimension;
    using geometry::vertices;
    auto geometry
     = [&](auto&& n) -> geometry_t<ad> { return grid.geometry(n); };

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
    for (auto d : ambient_dimension[grid]) {
      bounds_vtk[d] = geometry::x_min(bounds)(d);
      bounds_vtk[d] = geometry::x_max(bounds)(d);
    }
    unique_inserter->InitPointInsertion(points,
                                        static_cast<num_t*>(bounds_vtk));

    /// Create a temporary cell:
    log("Generating vtk grid...");
    auto tmp_cells = make_tuple_of_cells(grid_cell_t{});
    vector<int> cell_types;
    cell_types.reserve(no_cells);
    int_t c_c = 0;
    for (auto&& n : use_copy_if_single_pass(nodes)) {
      /// TODO: if constexpr(cell_geometry is variant (e.g. geometry::some))
      ///  - execute visit, and tuple of cells, ...
      ///  - otherwise: fast path, single geometry type will be outputed.
      auto cell_geometry = geometry(n);
      visit(
       [&](auto&& g) {
         using g_t             = decltype(g);
         using cell_write_type = cell_write_type_t<g_t>;
         if
           constexpr(Same<cell_write_type, vertex_based_cell>{}) {
             write_vertex_based_cell(g, tmp_cells, unique_inserter, cells,
                                     cell_types);
           }
         else if
           constexpr(Same<cell_write_type, face_based_cell>{}) {
             write_face_based_cell(g, tmp_cells, unique_inserter, cells,
                                   cell_types);
           }
         else {
           static_assert(
            always_false<g_t>{},
            "unknown vtk cell write type (vertex-based or face-based)");
         }
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

}  // namespace hm3::vis::vtk
#endif  // HM3_ENABLE_VTK
