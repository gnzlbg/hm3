#pragma once
/// \file
///
#include <hm3/solver/fv/state.hpp>
#include <hm3/vis/vtk/serialize.hpp>

namespace hm3 {
namespace solver {
namespace fv {

namespace vtk {

template <typename Physics, typename T>
struct serializable : dimensional<Physics::dimension()> {
  state<Physics> const& s;
  using block_idx = grid_node_idx;
  block_idx idx   = block_idx{};

  using vtk_cell_idx = cell_idx;

  auto geometry(cell_idx c) const noexcept { return s.geometry(c); }

  auto nodes() const noexcept {
    return s.all_cells() | view::filter([&](cell_idx c) {
             if (!idx) {
               return s.is_internal(c);
             } else {
               return s.is_in_block(c, idx);
             }
           });
  }

  auto bounding_box() const noexcept {
    return s.block_grid.tree().bounding_box();
  }

  template <typename F> auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }

  template <typename CellData> void load(CellData&& cell_data) const {
    cell_data.load("cell_idx", [&](cell_idx n, auto&&) {
      return n ? static_cast<int_t>(*n) : int_t{-1};
    });
    cell_data.load("block_idx", [&](cell_idx c, auto&&) {
      auto n = s.block(c);
      return n ? static_cast<int_t>(*n) : int_t{-1};
    });
    cell_data.load("block_cell_idx", [&](cell_idx c, auto&&) {
      auto n = s.block_cell_idx(c);
      return n;  // ? static_cast<int_t>(*n) : int_t{-1};
    });
    if (idx) {
      cell_data.load("is_internal", [&](cell_idx c, auto&&) {
        return static_cast<int_t>(s.is_internal(c));
      });
    }
    s.physics.load(T{s.physics}, s, cell_data);
  }

  serializable(state<Physics> const& s_, block_idx b = block_idx{})
   : s(s_), idx{b} {}
};

template <typename Physics, typename T>
void serialize(state<Physics> const& state, string file_name, T&&,
               grid_node_idx b = grid_node_idx{}) {
  using std::to_string;
  hm3::log::serial log("fv-serialization-to-vtk");

  if (b) { file_name += "_" + to_string(b); }

  serializable<Physics, T> s(state, b);
  ::hm3::vis::vtk::serialize(s, file_name, log);
}

}  // namespace vtk

}  // namespace fv
}  // namespace solver
}  // namespace hm3
