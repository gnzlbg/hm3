#pragma once
/// \file vtk.hpp Includes vtk headers and utilities
#ifdef HM3_ENABLE_VTK
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-semi"
#pragma clang diagnostic ignored "-Wfloat-equal"
// Memory management:
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
// Arrays of literal types:
#include <vtkCharArray.h>
#include <vtkIntArray.h>
#include <vtkLongArray.h>
#include <vtkLongLongArray.h>
#include <vtkUnsignedIntArray.h>
#include <vtkUnsignedLongArray.h>
#include <vtkUnsignedLongLongArray.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkStringArray.h>
// Cell Element types:
#include <vtkLine.h>
#include <vtkQuad.h>
#include <vtkHexahedron.h>
// Cell utilities:
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkMergePoints.h>
// Unstructured grid pipeline:
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridAlgorithm.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockDataSetAlgorithm.h>
// I/O
#include <vtkXMLUnstructuredGridWriter.h>
#pragma clang diagnostic pop
#include <string>
#include <hm3/geometry/square.hpp>

namespace hm3 {
namespace vis {

/// VTK functionality
namespace vtk {

template <class T> using smart_ptr = vtkSmartPointer<std::decay_t<T>>;

template <class T> auto make_ptr() -> vtkSmartPointer<std::decay_t<T>> {
  return vtkSmartPointer<std::decay_t<T>>::New();
}

template <class T> struct array {};
template <> struct array<char> { using type = vtkCharArray; };
template <> struct array<int> { using type = vtkIntArray; };
template <> struct array<long> { using type = vtkLongArray; };
template <> struct array<long long> { using type = vtkLongLongArray; };
template <> struct array<unsigned int> { using type = vtkUnsignedIntArray; };
template <> struct array<unsigned long> { using type = vtkUnsignedLongArray; };
template <> struct array<unsigned long long> {
  using type = vtkUnsignedLongLongArray;
};
template <> struct array<float> { using type = vtkFloatArray; };
template <> struct array<double> { using type = vtkDoubleArray; };
template <> struct array<string> { using type = vtkStringArray; };

template <class T> using array_t = typename array<T>::type;

template <class T> auto make_array() {
  return make_ptr<array_t<std::decay_t<T>>>();
}

template <class T> struct cell_t {};
template <> struct cell_t<geometry::square<1>> { using type = vtkLine; };
template <> struct cell_t<geometry::square<2>> { using type = vtkQuad; };
template <> struct cell_t<geometry::square<3>> { using type = vtkHexahedron; };

template <class T> using to_vtk_cell_t = typename cell_t<T>::type;

template <int_t nd>
constexpr auto to_vtk_point(geometry::point<nd> p) noexcept
 -> std::array<double, 3> {
  std::array<double, 3> vtk_p{{0.0}};
  for (uint_t d = 0; d != nd; ++d) { vtk_p[d] = p(d); }
  return vtk_p;
}

}  // namespace vtk

}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
