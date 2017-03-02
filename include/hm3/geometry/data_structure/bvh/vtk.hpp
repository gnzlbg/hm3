#pragma once
/// \file
///
/// Bounding Volume Hierarchy VTK serialization
#ifdef HM3_ENABLE_VTK
#include <hm3/geometry/data_structure/bvh/bvh.hpp>
#include <hm3/vis/vtk/serialize.hpp>

namespace hm3::geometry::bvh {

/// VTK visualization for Bounding-Volume Hierarchies.
namespace vtk {

template <typename State>
struct serializable : with_ambient_dimension<ad_v<State>> {
  State const& s;

  using vtk_cell_idx = node_idx;

  auto geometry(node_idx c) const noexcept { return s.bounding_box(c); }

  auto nodes() const noexcept { return s.node_ids(); }

  auto bounding_box() const noexcept {
    return bounding_volume.box(s.nodes_[0].bound);
  }

  template <typename F>
  auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }

  template <typename CellData>
  void load(CellData&& cell_data) const {
    cell_data.load("node_idx", [&](vtk_cell_idx n, auto&&) {
      return n ? static_cast<int_t>(*n) : int_t{-1};
    });
    cell_data.load("leaf", [&](vtk_cell_idx c, auto&&) {
      return s.is_leaf(c) ? int_t{1} : int_t{0};
    });
    cell_data.load("level",
                   [&](vtk_cell_idx c, auto&&) { return s.nodes_[*c].level; });
  }

  serializable(State const& s_) : s(s_) {}
};

template <typename State>
void serialize(State const& state, string file_name) {
  using std::to_string;
  hm3::log::serial log("bvh-serialization-to-vtk");

  serializable<State> s(state);
  ::hm3::vis::vtk::serialize(s, file_name, log);
}

}  // namespace vtk

}  // namespace hm3::geometry::bvh
#endif  // HM3_ENABLE_VTK
