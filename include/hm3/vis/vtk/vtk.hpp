#pragma once
/// \file
///
/// Common VTK headers and VTK utilities
#ifdef HM3_ENABLE_VTK
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-semi"
#pragma clang diagnostic ignored "-Wfloat-equal"
// Memory management:
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
// Arrays of literal types:
#include <vtkCharArray.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkLongArray.h>
#include <vtkLongLongArray.h>
#include <vtkStringArray.h>
#include <vtkUnsignedIntArray.h>
#include <vtkUnsignedLongArray.h>
#include <vtkUnsignedLongLongArray.h>
// Cell Element types:
#include <vtkHexahedron.h>
#include <vtkLine.h>
#include <vtkPolygon.h>
#include <vtkQuad.h>
// Cell utilities:
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkMergePoints.h>
// Unstructured grid pipeline:
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockDataSetAlgorithm.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridAlgorithm.h>
// I/O
#include <vtkXMLUnstructuredGridWriter.h>
#pragma clang diagnostic pop
// std
#include <string>
#include <tuple>
// hm3
#include <hm3/geometry/polygon.hpp>
#include <hm3/geometry/square.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3 {
namespace vis {

/// VTK functionality
namespace vtk {

template <typename T> using vtk_cell_idx_t = typename T::vtk_cell_idx;

template <typename T> using smart_ptr = vtkSmartPointer<std::decay_t<T>>;

template <typename T> auto make_ptr() -> vtkSmartPointer<std::decay_t<T>> {
  return vtkSmartPointer<std::decay_t<T>>::New();
}

template <typename T> struct array {};
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

template <typename T> using array_t = typename array<T>::type;

template <typename T> auto make_array() {
  return make_ptr<array_t<std::decay_t<T>>>();
}

template <typename T> struct cell_t {};
template <> struct cell_t<geometry::square<1>> {
  using type = vtkLine;
  static constexpr int value() { return VTK_LINE; }
};
template <> struct cell_t<geometry::square<2>> {
  using type = vtkQuad;
  static constexpr int value() { return VTK_QUAD; }
};
template <> struct cell_t<geometry::square<3>> {
  using type = vtkHexahedron;
  static constexpr int value() { return VTK_HEXAHEDRON; }
};
template <uint_t Nd, uint_t MaxNp> struct cell_t<geometry::polygon<Nd, MaxNp>> {
  using type = vtkPolygon;
  static constexpr int value() { return VTK_POLYGON; }
};

template <typename T> using to_vtk_cell_t     = typename cell_t<T>::type;
template <typename T> using to_vtk_cell_ptr_t = smart_ptr<to_vtk_cell_t<T>>;

template <typename... Ts>
using vtk_cell_types_t
 = meta::transform<meta::list<Ts...>, meta::quote<to_vtk_cell_t>>;

template <typename... Ts>
using vtk_cell_ptr_types_t
 = meta::transform<meta::list<Ts...>, meta::quote<to_vtk_cell_ptr_t>>;

template <typename... Ts>
using vtk_cell_ptr_tuple_t
 = meta::apply_list<meta::quote<std::tuple>, vtk_cell_ptr_types_t<Ts...>>;

template <typename... Ts>
auto make_tuple_of_cells(std::experimental::variant<Ts...> const&)
 -> vtk_cell_ptr_tuple_t<Ts...> {
  vtk_cell_ptr_tuple_t<Ts...> r;
  ranges::tuple_for_each(
   r, [](auto&& t) { t = std::decay_t<decltype(t)>::New(); });
  return r;
}

template <uint_t Nd>
constexpr auto to_vtk_point(geometry::point<Nd> p) noexcept
 -> std::array<double, 3> {
  std::array<double, 3> vtk_p{{0.0}};
  for (uint_t d = 0; d != Nd; ++d) { vtk_p[d] = p(d); }
  return vtk_p;
}

}  // namespace vtk

}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
