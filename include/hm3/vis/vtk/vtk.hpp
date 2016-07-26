#pragma once
/// \file
///
/// Common VTK headers and VTK utilities
#ifdef HM3_ENABLE_VTK
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-semi"
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wdocumentation"
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
#include <hm3/geometry/box.hpp>
#include <hm3/geometry/polygon.hpp>
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
template <> struct cell_t<geometry::box<1>> {
  using type = vtkLine;
  static constexpr auto value() { return VTK_LINE; }
};
template <> struct cell_t<geometry::box<2>> {
  using type = vtkQuad;
  static constexpr auto value() { return VTK_QUAD; }
};
template <> struct cell_t<geometry::box<3>> {
  using type = vtkHexahedron;
  static constexpr auto value() { return VTK_HEXAHEDRON; }
};
template <> struct cell_t<geometry::aabb<1>> {
  using type = vtkLine;
  static constexpr auto value() { return VTK_LINE; }
};
template <> struct cell_t<geometry::aabb<2>> {
  using type = vtkQuad;
  static constexpr auto value() { return VTK_QUAD; }
};
template <> struct cell_t<geometry::aabb<3>> {
  using type = vtkHexahedron;
  static constexpr auto value() { return VTK_HEXAHEDRON; }
};

template <dim_t Nd, suint_t MaxNp> struct cell_t<geometry::polygon<Nd, MaxNp>> {
  using type = vtkPolygon;
  static constexpr auto value() { return VTK_POLYGON; }
};

template <typename T> using to_vtk_cell_t     = typename cell_t<T>::type;
template <typename T> using to_vtk_cell_ptr_t = smart_ptr<to_vtk_cell_t<T>>;

template <typename... Ts>
using vtk_cell_types_t
 = meta::unique<meta::transform<meta::list<Ts...>, meta::quote<to_vtk_cell_t>>>;

template <typename... Ts>
using vtk_cell_ptr_types_t = meta::
 unique<meta::transform<meta::list<Ts...>, meta::quote<to_vtk_cell_ptr_t>>>;

template <typename... Ts>
using vtk_cell_ptr_tuple_t
 = meta::apply<meta::quote<std::tuple>, vtk_cell_ptr_types_t<Ts...>>;

template <typename... Ts>
auto make_tuple_of_cells(std::experimental::variant<Ts...> const&)
 -> vtk_cell_ptr_tuple_t<Ts...> {
  vtk_cell_ptr_tuple_t<Ts...> r;
  ranges::tuple_for_each(
   r, [](auto&& t) { t = std::decay_t<decltype(t)>::New(); });
  return r;
}

using vtk_point_t = ::hm3::array<double, 3>;

template <dim_t Nd>
constexpr auto to_vtk_point(geometry::point<Nd> p) noexcept -> vtk_point_t {
  vtk_point_t vtk_p{{0.0, 0.0, 0.0}};
  for (dim_t d = 0; d < Nd; ++d) { vtk_p[d] = p(d); }
  return vtk_p;
}

}  // namespace vtk

}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
