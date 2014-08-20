#pragma once
/// \file serialize.hpp Serialization utilities for AMR state
///
/// Note: to use the serialize functionality one needs to link against VTK
///
#ifdef HM3_ENABLE_VTK
#include <hm3/amr/state.hpp>
#include <hm3/vis/vtk/unstructured_grid.hpp>
#include <hm3/vis/vtk/cell_data.hpp>

namespace hm3 {
namespace amr {

namespace vtk {

namespace concepts {
namespace rc = ranges::concepts;

/// SerializableToVTK concept
struct serializable_to_vtk {
  template <typename T>
  auto requires_(T&& t) -> decltype(rc::valid_expr(      //
   (t.bounding_box()),                                   //
   (t.dimensions()),                                     //
   (t.geometry(amr_node_idx_t<ranges::uncvref_t<T>>{}))  //
   //
   ));
};

}  // namespace concepts

template <typename T>
using SerializableToVTK
 = concepts::rc::models<concepts::serializable_to_vtk, ranges::uncvref_t<T>>;

/// Adapts an amr::state<Target> to be serializable to a vtkUnstructuredGrid
template <class Target> struct serializable {
  static_assert(SerializableToVTK<Target>{},
                "Target is not serializable to VTK");
  state<Target> const& s_;

  using cell_data_idx = amr_node_idx_t<Target>;

  serializable(state<Target> const& s) : s_{s} {}

  template <class NodeIdx> auto geometry(NodeIdx&& n) const {
    return s_.target().geometry(std::forward<NodeIdx>(n));
  }

  auto nodes() const {
    return s_.nodes() | view::transform([&](auto&& i) { return i.idx; });
  }

  auto bounding_box() const { return s_.target().bounding_box(); }

  auto dimensions() const { return s_.target().dimensions(); }

  template <class F> auto for_each_cell(F&& f) const {
    f(nodes());
    return f;
  }

  template <class T> auto find(T&& t) { return s_.find(t); }
};

template <class Target, CONCEPT_REQUIRES_(SerializableToVTK<Target>{})>
void serialize(state<Target> const& amr_state, string fname_) {
  string fname = fname_ + ".vtu";
  hm3::log::serial log("amr-serialization-to-vtk");

  // makes an amr_state serializable if its target is serializable
  serializable<Target> state(amr_state);

  vis::vtk::unstructured_grid vtk_grid;
  vtk_grid.log = log;

  vtk_grid.reinitialize(state.nodes(), state);

  using cell_data_t
   = vis::vtk::cell_data<serializable<Target>, vis::vtk::unstructured_grid>;
  cell_data_t cell_data(state, vtk_grid, log);

  cell_data.load("action", [&](auto&& n, auto&&) {
    auto i = state.find(n);
    HM3_ASSERT(i, "??");
    return (int)(i->action);
  });
  cell_data.load("idx", [&](auto&& n, auto&&) {
    auto i = state.find(n);
    HM3_ASSERT(i, "??");
    return *(i->idx);
  });

  vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer
   = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
  writer->SetFileName(fname.c_str());
  writer->SetInputData(vtk_grid.vtk_grid);
  writer->Write();
}

}  // namespace vtk
}  // namespace amr
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
