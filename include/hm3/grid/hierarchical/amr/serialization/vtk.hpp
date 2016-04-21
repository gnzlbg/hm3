#pragma once
/// \file
///
/// Serialization utilities for AMR state
///
/// Note: to use the serialize functionality one needs to link against VTK
///
#ifdef HM3_ENABLE_VTK
#include <hm3/geometry/dimensions.hpp>
#include <hm3/grid/hierarchical/amr/state.hpp>
#include <hm3/vis/vtk/geometry.hpp>
#include <hm3/vis/vtk/serialize.hpp>

namespace hm3 {
namespace grid {
namespace hierarchical {
namespace amr {

namespace vtk {

/// Adapts an amr::state<Target> to be serializable to a vtkUnstructuredGrid
template <typename Target>
struct serializable : geometry::dimensional<Target::dimension()> {
  state<Target> const& s_;

  using vtk_cell_idx = amr_node_idx_t<Target>;

  serializable(state<Target> const& s) : s_{s} {}

  template <typename NodeIdx> auto geometry(NodeIdx&& n) const {
    return s_.target().geometry(std::forward<NodeIdx>(n));
  }

  auto nodes() const {
    return s_.nodes() | view::transform([&](auto&& i) { return i.idx; });
  }

  auto bounding_box() const { return s_.target().bounding_box(); }

  template <typename F> auto for_each_cell(F&& f) const {
    f(nodes());
    return f;
  }

  template <typename CellData> void load(CellData&& cell_data) const {
    cell_data.load("action", [&](auto&& n, auto&&) {
      auto i = s_.find(n);
      HM3_ASSERT(i, "??");
      return static_cast<sint_t>(i->action);
    });
    cell_data.load("idx", [&](auto&& n, auto&&) {
      auto i = s_.find(n);
      HM3_ASSERT(i, "??");
      return *(i->idx);
    });
  }
};

template <typename Target>
void serialize(state<Target> const& amr_state, string fname_) {
  hm3::log::serial log("amr-serialization-to-vtk");

  // makes an amr_state serializable if its target is serializable
  serializable<Target> state(amr_state);
  ::hm3::vis::vtk::serialize(state, fname_, log);
}

}  // namespace vtk
}  // namespace amr
}  // namespace hierarchical
}  // namespace grid
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
