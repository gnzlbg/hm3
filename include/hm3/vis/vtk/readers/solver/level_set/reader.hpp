#pragma once
/// \file
///
/// VTK level-set reader
#ifdef HM3_ENABLE_VTK
#include <hm3/solver/level_set/state.hpp>
#include <hm3/vis/vtk/cell_data.hpp>
#include <hm3/vis/vtk/reader.hpp>
#include <hm3/vis/vtk/readers/grid/reader.hpp>

namespace hm3 {
namespace vis {
namespace vtk {
namespace level_set {

template <dim_t Nd> struct reader : ::hm3::vis::vtk::reader {
  using grid_reader_t = ::hm3::vis::vtk::grid::reader<Nd>;
  using level_set_s   = ::hm3::solver::level_set::state<Nd>;

  using vtk_cell_idx = typename level_set_s::cell_idx;

  /// Pointer to the grid reader (a dependency of level set reader)
  grid_reader_t const* grid_reader;

  /// Cached VTK grid:
  unstructured_grid vtk_grid;
  using cell_data_t = cell_data<reader<Nd>, unstructured_grid>;

  /// Cell Data handler:
  cell_data_t cell_data;

  /// Level-Set state
  std::unique_ptr<level_set_s> level_set = nullptr;

  template <typename F> auto for_each_cell(F&& f) const noexcept {
    f(level_set->g.in_use());
    return std::forward<F>(f);
  }

 private:
  virtual void initialize_() override final {
    log("Registering fields...");

    vtk_grid.log = log;
    cell_data    = cell_data_t(*this, vtk_grid);

    HM3_ASSERT(io_.has_dependency(), "level-set reader depends on the grid");
    HM3_ASSERT(readers_ != nullptr, "pointer to readers table not set");
    grid_reader
     = dynamic_cast<grid_reader_t const*>((*readers_)[io_.dependency()].get());

    HM3_ASSERT(
     grid_reader != nullptr,
     "Error: getting the grid reader of the level-set reader failed!");

    register_cell_data();
    for (auto&& f : cell_data) { log("  - {}", f.name); }
    log("...registering fields done!");

    status = reader_status::topology_changed;
  }

  void register_cell_data() noexcept {
    /// Node index:
    cell_data.push("level_set_idx", [&](auto&& i) { return *i; });

    cell_data.push("level_set_value",
                   [&](auto&& i) { return level_set->signed_distance(i); });

    if (!cell_data.assert_unique()) {
      HM3_FATAL_ERROR("fields are not unique!");
    }
  }

  void update_(vtkMultiBlockDataSet* mb, int file_idx) override final {
    HM3_ASSERT(status != reader_status::uninitialized,
               "trying to update an uninitialized reader");

    // update_reader_status();  // visualization level
    load_file(file_idx);

    if (status == reader_status::topology_changed) {
      log("Reader grid topology changed...");
      cell_data.unload_all();
      for_each_cell(
       [&](auto&& nodes) { vtk_grid.reinitialize(nodes, *level_set); });
      status = reader_status::topology_ready;
    }

    cell_data.update();

    {  // debug
      auto g = vtk_grid.vtk_grid.Get();
      if (g->GetCellData()->HasArray("level_set_value")) {
        log("has level-set_value array!");
      } else {
        log("does not have level-set_value array!");
      }
    }

    mb->SetBlock(block_idx_, vtk_grid.vtk_grid.Get());
    mb->GetMetaData(block_idx_)
     ->Set(vtkCompositeDataSet::NAME(), io_.name().c_str());
  }

  void load_file_(int file_idx) override final {
    auto&& gp = grid_reader->get_grid();
    HM3_ASSERT(gp != nullptr, "cannot load file because there is no grid");
    HM3_ASSERT(file_idx != -1, "invalid file index");
    level_set = std::make_unique<level_set_s>(level_set_s::from_session(
     io_.session(), *gp, io_.name(), io::file::index_t{file_idx}));
  }

  bool set_visualization_level_(const int /* level*/) noexcept override final {
    return false;
  }

  bool field_(string const& name, const int value) noexcept override final {
    return cell_data(name, value);
  }

  int field_(string const& name) const noexcept override final {
    return cell_data(name);
  }

  const char* field_(const int index) const noexcept override final {
    return cell_data(index);
  }

  int number_of_fields_() const noexcept override final {
    return cell_data.size();
  }
};

/// Makes a level_set reader of dimension `Nd`
template <dim_t Nd>
auto make_level_set() -> std::unique_ptr<::hm3::vis::vtk::reader> {
  return std::make_unique<reader<Nd>>();
}

/// Makes a level_set reader from io::session block of type
/// level_set.
inline auto make(io::json const& b)
 -> std::unique_ptr<::hm3::vis::vtk::reader> {
  string block_type = io::read_field(b, "type", HM3_AT_);
  if (block_type != "level_set") {
    HM3_FATAL_ERROR(
     "Cannot make a level_set reader from a block of type \"{}\"", block_type);
  }
  dim_t nd = io::read_file_field(b, "spatial_dimension", HM3_AT_);
  switch (nd) {
    case 1: {
      return make_level_set<1>();
    }
    case 2: {
      return make_level_set<2>();
    }
    case 3: {
      return make_level_set<3>();
    }
    default: {
      HM3_FATAL_ERROR("unsupported #of spatial dimensions, nd = {}", nd);
    }
  }
}

}  // namespace level_set
}  // namespace vtk
}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
