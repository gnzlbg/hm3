#pragma once
/// \file
///
/// VTK Reader for grid::hierarchical::cartesian::multi
#ifdef HM3_ENABLE_VTK
#include <hm3/vis/vtk/readers/grid/tree.hpp>

namespace hm3 {
namespace vis {
namespace vtk {
namespace grid {

/// Multi hierarchical Cartesian grid reader
template <dim_t Nd>
struct multi : tree<Nd> {
  using tree<Nd>::grid;
  using grid_idx = ::hm3::hierarchical::grid_idx;
  using gidx_t   = ::hm3::hierarchical::gidx_t;

  using tree<Nd>::cell_data;

  multi(gidx_t no_grids) : no_grids_(no_grids) {}

 protected:
  gidx_t no_grids_ = 0;

  /// Register hierarchical Cartesian grid fields
  virtual void register_cell_data() noexcept override {
    if (grid) {
      HM3_ASSERT(no_grids_ == *grid->no_grids(),
                 "#of grid mismatch (in multi reader: {}, in readed grid: {})",
                 no_grids_, grid->no_grids());
    }
    for (gidx_t g = 0; g != no_grids_; ++g) {
      cell_data.push("grid_" + std::to_string(g) + "_idx", [&, g](auto&& i) {
        auto v = grid->node(i, grid_idx{g});
        return v ? *v : -1;
      });
    }

    tree<Nd>::register_cell_data();
  }
};

/// Makes a multi hierarchical Cartesian grid reader of dimension `Nd`
template <dim_t Nd>
auto make_multi(hierarchical::gidx_t ng)
 -> std::unique_ptr<::hm3::vis::vtk::reader> {
  return std::make_unique<multi<Nd>>(ng);
}

/// Makes a multi hierarchical Cartesian grid reader from a io::session block of
/// type "multi hierarchical Cartesian".
inline auto make_multi(io::json const& b) {
  string block_type = io::read_field(b, "type", HM3_AT_);
  if (block_type != multi_type()) {
    HM3_FATAL_ERROR("Cannot 1make a hm3::grid::hierarchical::cartesian::multi "
                    "reader from a block of type \"{}\"",
                    block_type);
  }

  if (b["files"].empty()) {
    HM3_FATAL_ERROR(
     "multi hierarchical Cartesian grid contains no files:\n\n{}\n\n", b);
  }

  dim_t nd = io::read_file_field(b, "spatial_dimension", HM3_AT_);
  hierarchical::gidx_t ng = io::read_file_field(b, "no_grids", HM3_AT_);

  switch (nd) {
    case 1: {
      return make_multi<1>(ng);
    }
    case 2: {
      return make_multi<2>(ng);
    }
    case 3: {
      return make_multi<3>(ng);
    }
    default: {
      HM3_FATAL_ERROR("unsupported #of spatial dimensions, nd = {}", nd);
    }
  }
}

}  // namespace grid
}  // namespace vtk
}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
