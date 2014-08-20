#pragma once
/// \file
///
/// VTK cell data
#ifdef HM3_ENABLE_VTK
#include <hm3/vis/vtk/data_fields.hpp>
#include <hm3/vis/vtk/vtk.hpp>
#include <hm3/utility/log.hpp>

namespace hm3 {
namespace vis {
namespace vtk {

/// Manages cell data fields in a vtk grid "automatically"
///
/// Each data field in a reader needs to provide its name, the number of
/// components it has (for multi-dimensional fields), the name of the
/// components, and how to obtain/compute the data for a given cell.
///
/// Each reader needs to provide:
/// - a type of the cell indices: cell_data_idx used to index the cell data
/// - a function to execute a kernel over all reader cells:
///     `auto for_each_cell(UnaryFunction(CellRange) uf) ->
///     UnaryFunction(CellRange)`
///   where CellRange is a range of cells.
///
/// To be as fast as possible:
/// - the lambda has to be inlined (it cannot be a virtual function,
///   function pointer, or type-erased via a std::function),
/// - the range of cells has to be inlined (it cannot be an any_range).
///
/// To store multiple fields that compute themself differently:
/// - we need to erase the types of the fields at some point.
///
/// This class achieves the "to be as fast as possible" part, while its parent
/// class data_fields achieves the "store multiple fields part".
///
/// In a nutshell: each fields knows how to load itself. This is what the member
/// variable of field load : std::function<void(void)> does, but inside load the
/// types of the cell range and the kernel to obtain the data are known, so the
/// loading loop can be inlined and vectorized.
///
/// The readers add a field to data_fields by calling push with: the field name,
/// the number of components, the component names, how to load the field for a
/// given cell... Push then creates a field object that knows how to load
/// itself, by using cell_data::load to abstract obtaining the correct range of
/// cells from the reader, and the location to write the output from a vtk_grid.
///
/// That is, loading a field involves a virtual function call (field::load), but
/// inside this function all the types are known.
///
/// This is a bit messy and should be simplified, but it works.
///
template <typename Reader, typename VTKGrid>
struct cell_data : data_fields<cell_data<Reader, VTKGrid>> {
  using cell_data_idx  = typename Reader::cell_data_idx;
  Reader const* reader = nullptr;
  VTKGrid* vtk_grid_   = nullptr;
  log::serial log;

  cell_data() = default;

  cell_data(Reader const& r, VTKGrid& vtk_g)
   : reader{&r}, vtk_grid_{&vtk_g}, log{reader->log} {}

  cell_data(Reader const& r, VTKGrid& vtk_g, log::serial l)
   : reader{&r}, vtk_grid_{&vtk_g}, log{std::move(l)} {}

  /// Get the output vtk_grid
  auto vtk_grid() const noexcept -> decltype(vtk_grid_->vtk_grid) {
    HM3_ASSERT(vtk_grid_, "cell data is not initialized!");
    return vtk_grid_->vtk_grid;
  }
  /// Get the output vtk_grid
  auto vtk_grid() noexcept -> decltype(vtk_grid_->vtk_grid) {
    HM3_ASSERT(vtk_grid_, "cell data is not initialized!");
    return vtk_grid_->vtk_grid;
  }

  /// Is cell data field \p f loaded in grid \p vtk_grid?
  bool loaded(field const& f) const noexcept {
    auto n = f.name;
    // if there is no grid, no field can be loaded, otherwise check if there
    // is an array in the vtk grid
    return vtk_grid() ? vtk_grid()->GetCellData()->HasArray(n.c_str()) : false;
  }

  /// Unload cell data field \p f from grid \p vtk_grid
  void unload(field const& f) {
    HM3_ASSERT(loaded(f), "cannot unload: field is not loaded!");
    auto n = f.name;
    vtk_grid()->GetCellData()->RemoveArray(n.c_str());
  }

  /// Load field with \p field_name into the vtk_grid.
  ///
  /// \param [in] field_name The name of the field
  /// \param [in] no_components The number of components/dimensions of the field
  /// \param [in] field_component Function that returns for a given node and
  ///             component the data of the component:
  ///             field_component(node, component_idx) -> component_value
  ///
  /// \param [in] component_name Function that returns for a given
  ///             component/dimensions its name:
  ///             component_name(component_idx) -> string.
  ///
  template <typename FieldComponentValue,
            typename ComponentName = std::function<string(int)>>
  void load(string const& field_name, FieldComponentValue&& field_component,
            const sint_t no_components = 1,
            ComponentName&& component_name
            = [](int c) { return std::to_string(c); }) {
    HM3_ASSERT(reader, "reader pointer not set in cell data");
    // type of the component data
    using data_t
     = decltype(field_component(std::declval<cell_data_idx>(), sint_t{}));
    // array to store the component values
    auto data = make_array<data_t>();
    data->SetName(field_name.c_str());
    data->SetNumberOfComponents(no_components);

    log("Loading field {} with {} components...", field_name, no_components);

    const auto components = view::ints(sint_t{0}, sint_t{no_components});
    if (no_components > 1) {
      for (auto&& c : components) {
        const auto n = component_name(c);
        log("  - component {}: {}", c, n);
        data->SetComponentName(c, n.c_str());
      }
    }

    auto array_size = vtk_grid()->GetNumberOfCells() * no_components;
    log("Allocating {} values...", array_size);
    data->Allocate(array_size);
    data->SetNumberOfTuples(vtk_grid()->GetNumberOfCells());

    // Calls the reader for_each_cell function, which passes the range of
    // noces/cells on which the data has to be computed/loaded:
    // These range can have different types (for nodes in the grid, for nodes
    // at a different grid level, ...).
    int_t c_c = 0;
    reader->for_each_cell([&](auto&& nodes) {
      std::size_t v_c = 0;
      RANGES_FOR (auto&& n, nodes) {
        for (auto&& c : components) {
          data->SetComponent(v_c, c, field_component(n, c));
          ++c_c;
        }
        ++v_c;
      }
      log("Set {} values and {} components...", v_c, c_c);
    });
    HM3_ASSERT(
     c_c == array_size,
     "size mismatch: {} allocated components but {} components were set",
     array_size, c_c);

    // Add the cell data array to the vtk grid
    auto index = vtk_grid()->GetCellData()->AddArray(data);
    HM3_ASSERT(static_cast<vtkDataArray*>(data.Get())
                == vtk_grid()->GetCellData()->GetArray(index),
               "setting the array did not work??");
    log("Added data array for field {} at index {}...", field_name, index);
    {
      // Assert that the array sizes/bounds are fine.
      ///
      /// Setting the number of tuples should not be required.
      //
      // Note: #of components + #of cells is enough when using a vtkUnstructured
      // grid outside of a vtkMultiBlockDataSet. When including it in the
      // vtkMultiBlockDataSet, paraview crashes because the #of tuples is zero.
      // TODO: report ParaView bug?
      auto no_tuples
       = vtk_grid()->GetCellData()->GetArray(index)->GetNumberOfTuples();
      auto no_cells = vtk_grid()->GetNumberOfCells();
      HM3_ASSERT(no_tuples == no_cells, "#of tuples = {}, #of cells = {}",
                 no_tuples, no_cells);
      auto max_id = vtk_grid()->GetCellData()->GetArray(index)->GetMaxId();
      HM3_ASSERT(max_id == array_size - 1, "max_id = {}, array size = {}",
                 max_id, array_size);
      auto size = vtk_grid()->GetCellData()->GetArray(index)->GetSize();
      HM3_ASSERT(size == array_size, "size = {}, array size = {}", size,
                 array_size);
    }
    log("... field {} loaded!", field_name);
  }
};

}  // namespace vtk
}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
