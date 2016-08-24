#pragma once
/// \file
///
/// Serialization to a VTK unstructured grid
#ifdef HM3_ENABLE_VTK

/// Define this to switch to ASCII output format for VTK files
//#define HM3_VTK_OUTPUT_ASCII

#include <hm3/vis/vtk/cell_data.hpp>
#include <hm3/vis/vtk/unstructured_grid.hpp>

namespace hm3 {
namespace vis {
namespace vtk {

namespace concepts {
namespace rc = ranges::concepts;

/// SerializableToVTK concept
struct serializable_to_vtk {
  struct unary_fn {
    template <typename TRange>
    void operator()(TRange&&) const {}
  };

  template <typename T,                           //
            typename U   = ranges::uncvref_t<T>,  //
            typename UG  = unstructured_grid,     //
            typename CD  = cell_data<U, UG>,      //
            typename Idx = vtk_cell_idx_t<U>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(  //
   (t.bounding_box()),                               //
   (t.dimensions()),                                 //
   (t.geometry(Idx{})),                              //
   //(t.load(CD())),                                 //
   (t.for_each_cell(unary_fn{}))  //
   ));
};

}  // namespace concepts

template <typename T>
using SerializableToVTK
 = concepts::rc::models<concepts::serializable_to_vtk, ranges::uncvref_t<T>>;

template <typename Source, CONCEPT_REQUIRES_(SerializableToVTK<Source>{})>
void serialize(Source const& s, string file_name_, log::serial log) {
  string file_name = file_name_ + ".vtu";

  vis::vtk::unstructured_grid vtk_grid;
  vtk_grid.log = log;  // TODO: log repetition

  vtk_grid.reinitialize(s.nodes(), s);

  using cell_data_t = vis::vtk::cell_data<Source, vis::vtk::unstructured_grid>;
  cell_data_t cell_data(s, vtk_grid, log);  // TODO: log repetition

  s.load(cell_data);

  vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer
   = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
  writer->SetFileName(file_name.c_str());
  writer->SetInputData(vtk_grid.vtk_grid);
#ifdef HM3_VTK_OUTPUT_ASCII
  writer->SetDataMode(vtkXMLWriter::Ascii);
#else
  writer->SetDataMode(vtkXMLWriter::Binary);
#endif  // HM3_VTK_OUTPUT_ASCII
  writer->Write();
}

}  // namespace vtk
}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
