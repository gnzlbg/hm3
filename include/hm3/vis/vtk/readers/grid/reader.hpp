#pragma once
/// \file
///
/// VTK Grid reader
#ifdef HM3_ENABLE_VTK
#include <hm3/geometry/square.hpp>
#include <hm3/grid/hierarchical/cartesian/multi.hpp>
#include <hm3/grid/hierarchical/cartesian/serialization/multi_vtk.hpp>
#include <hm3/grid/hierarchical/types.hpp>
#include <hm3/vis/vtk/readers/grid/types.hpp>
/// VTK:
#include <hm3/vis/field.hpp>
#include <hm3/vis/vtk/cell_data.hpp>
#include <hm3/vis/vtk/reader.hpp>
#include <hm3/vis/vtk/unstructured_grid.hpp>
#include <hm3/vis/vtk/vtk.hpp>
/// STD:
#include <memory>
#include <vector>

namespace hm3 {
namespace vis {
namespace vtk {
namespace grid {

/// Common VTK Grid reader functionality
///
/// Implements most of the functionality required to read a hierarchical grid.
/// For the specific parts see each reader:
/// - tree
/// - single (single hierarchical Cartesian grid)
/// - multi (multiple hierarchical Cartesian grids)
///
/// All grids are stored as a multi_hierarchical_cartesian_grid. Trees are
/// loaded with a root cell of length 1 centered at (0.5, ..., 0.5).
///
/// The current vtk grid displayed as well as the cell data
/// are cached inside the reader.
///
template <uint_t Nd> struct reader : ::hm3::vis::vtk::reader {
  using grid_t       = ::hm3::grid::hierarchical::cartesian::multi<Nd>;
  using vtk_cell_idx = tree::node_idx;

 protected:
  /// Hierarchical Cartesian Grid
  std::unique_ptr<grid_t> grid = nullptr;

  /// Cached VTK grid:
  unstructured_grid vtk_grid;
  using cell_data_t = cell_data<reader<Nd>, unstructured_grid>;
  /// Cell Data handler:
  cell_data_t cell_data;

 private:
  /// Visualization level:
  int future_visualization_level_ = -1;
  int past_visualization_level_   = -1;

 public:
  /// Set the visualization level to /p value (returns true if changed)
  bool set_visualization_level_(const int value) noexcept override final {
    future_visualization_level_ = value;
    return future_visualization_level_ != past_visualization_level_;
  }

 public:
  reader()         = default;
  ~reader()        = default;
  reader(reader&&) = default;
  reader& operator=(reader&&) = default;

  // Non-copyable
  reader(reader const&) = delete;
  reader& operator=(reader const&) = delete;

  /// Pointer to the hierarchical Cartesian grid stored
  std::unique_ptr<grid_t> const& get_grid() const noexcept { return grid; }

 public:
  /// \name Cell data field access
  ///@{

  /// Enable/disable cell data
  bool field_(string const& name, const int value) noexcept override final {
    return cell_data(name, value);
  }

  /// Index of cell data named \p name
  int field_(string const& name) const noexcept override final {
    return cell_data(name);
  }

  /// Name of field at \p index
  const char* field_(const int index) const noexcept override final {
    return cell_data(index);
  }

  /// Number of cell data fields
  int number_of_fields_() const noexcept override final {
    return cell_data.size();
  }

  ///@}  // Cell data field access

  void initialize_() override final {
    HM3_ASSERT(status == reader_status::uninitialized,
               "trying to initialize an already initialized reader");
    vtk_grid.log = log;
    cell_data    = cell_data_t(*this, vtk_grid);

    log("Registering fields...");
    register_cell_data();
    for (auto&& f : cell_data) { log("  - {}", f.name); }
    log("...registering fields done!");

    status = reader_status::topology_changed;
  }

  virtual void register_cell_data() noexcept {
    if (!cell_data.assert_unique()) {
      HM3_FATAL_ERROR("fields are not unique!");
    }
  }

  template <typename F> auto for_each_cell(F&& f) const noexcept {
    log("Applying to visualization level {}...", visualization_level());
    if (visualization_level() < 0) {
      f(grid->nodes() | grid->leaf());
    } else {
      auto level = static_cast<int>(visualization_level());
      f(grid->nodes() | tree::node_level.filter(*grid, level));
    }
    log("... applying done!");
    return std::forward<F>(f);
  }

 private:
  // Updates the visualization level (returns true if it changed)
  bool update_visualization_level() noexcept {
    if (future_visualization_level_ == past_visualization_level_) {
      return false;
    }
    log("visualization level changed from {} to {}", past_visualization_level_,
        future_visualization_level_);
    past_visualization_level_ = future_visualization_level_;
    return true;
  }

  int visualization_level() const noexcept {
    return future_visualization_level_;
  }

  void update_reader_status() {
    log("Updating reader status...");
    if (update_visualization_level()) {
      status = reader_status::topology_changed;
    }
    log("...reader status updated!");
  }

  void load_file_(int file_idx) override final {
    if (io_.type() == tree_type() || io_.type() == single_type()
        || io_.type() == multi_type()) {
      grid = std::make_unique<grid_t>(
       grid_t::from_session(io_.session(), io_.type(), io_.name(), file_idx));
    } else {
      HM3_FATAL_ERROR(
       "grid reader supports only hierarchical_cartesian and hyperoctree");
    }
  }

 public:
  void update_(vtkMultiBlockDataSet* mb, int new_file_idx) override final {
    HM3_ASSERT(status != reader_status::uninitialized,
               "trying to update an uninitialized reader");

    update_reader_status();
    load_file(new_file_idx);

    if (status == reader_status::topology_changed) {
      log("Reader grid topology changed...");
      cell_data.unload_all();

      ::hm3::hierarchical::cartesian::vtk::serializable_multi<Nd> s(*grid);

      for_each_cell([&](auto&& nodes) { vtk_grid.reinitialize(nodes, s); });
      status = reader_status::topology_ready;
    }

    cell_data.update();

    mb->SetBlock(block_idx_, vtk_grid.vtk_grid.Get());
    mb->GetMetaData(block_idx_)
     ->Set(vtkCompositeDataSet::NAME(), io_.name().c_str());
  }
};

}  // namespace grid
}  // namespace vtk
}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
