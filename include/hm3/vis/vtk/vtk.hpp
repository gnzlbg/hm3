#pragma once
/// \file
///
/// Common VTK headers and VTK utilities
#ifdef HM3_ENABLE_VTK
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-semi"
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
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
#include <vtkPolyLine.h>
#include <vtkPolygon.h>
#include <vtkPolyhedron.h>
#include <vtkQuad.h>
#include <vtkVertex.h>
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
// hm3
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/some.hpp>
#include <hm3/utility/tuple.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::vis {

/// VTK functionality
namespace vtk {

template <typename T>
using vtk_cell_idx_t = typename T::vtk_cell_idx;

template <typename T>
using smart_ptr = vtkSmartPointer<uncvref_t<T>>;

template <typename T>
auto make_ptr() -> vtkSmartPointer<uncvref_t<T>> {
  return vtkSmartPointer<uncvref_t<T>>::New();
}

template <typename T>
struct array {};
template <>
struct array<char> {
  using type = vtkCharArray;
};
template <>
struct array<int> {
  using type = vtkIntArray;
};
template <>
struct array<long> {
  using type = vtkLongArray;
};
template <>
struct array<long long> {
  using type = vtkLongLongArray;
};
template <>
struct array<unsigned int> {
  using type = vtkUnsignedIntArray;
};
template <>
struct array<unsigned long> {
  using type = vtkUnsignedLongArray;
};
template <>
struct array<unsigned long long> {
  using type = vtkUnsignedLongLongArray;
};
template <>
struct array<float> {
  using type = vtkFloatArray;
};
template <>
struct array<double> {
  using type = vtkDoubleArray;
};
template <>
struct array<string> {
  using type = vtkStringArray;
};

template <typename T>
using array_t = typename array<T>::type;

template <typename T>
auto make_array() {
  return make_ptr<array_t<uncvref_t<T>>>();
}

struct vertex_based_cell {};
struct face_based_cell {};

struct line {
  using type      = vtkLine;
  using cell_type = vertex_based_cell;
  static constexpr auto value() { return VTK_LINE; }
};

struct quad {
  using type      = vtkQuad;
  using cell_type = vertex_based_cell;
  static constexpr auto value() { return VTK_QUAD; }
};

struct hexahedron {
  using type      = vtkHexahedron;
  using cell_type = vertex_based_cell;
  static constexpr auto value() { return VTK_HEXAHEDRON; }
};

struct polygon {
  using type      = vtkPolygon;
  using cell_type = vertex_based_cell;
  static constexpr auto value() { return VTK_POLYGON; }
};

struct polyline {
  using type      = vtkPolyLine;
  using cell_type = vertex_based_cell;
  static constexpr auto value() { return VTK_POLY_LINE; }
};

struct polyhedron {
  using type      = vtkPolyhedron;
  using cell_type = face_based_cell;
  static constexpr auto value() { return VTK_POLYHEDRON; }
};

struct vertex {
  using type      = vtkVertex;
  using cell_type = vertex_based_cell;
  static constexpr auto value() { return VTK_VERTEX; }
};

template <typename T, CONCEPT_REQUIRES_(geometry::Point<uncvref_t<T>>{})>
constexpr auto cell_type(T&&) {
  return vertex{};
}

template <typename T, CONCEPT_REQUIRES_(geometry::Segment<uncvref_t<T>>{})>
constexpr auto cell_type(T&&) {
  return line{};
}

template <typename T, CONCEPT_REQUIRES_(geometry::Polygon<uncvref_t<T>>{})>
constexpr auto cell_type(T&&) {
  if
    constexpr(geometry::AABB<T, 2>{} or geometry::Box<T, 2>{}) {
      return quad{};
    }
  else {
    return polygon{};
  }
}

template <typename T, CONCEPT_REQUIRES_(geometry::Polyline<uncvref_t<T>>{})>
constexpr auto cell_type(T&&) {
  return polyline{};
}

template <typename T, CONCEPT_REQUIRES_(geometry::Polyhedron<uncvref_t<T>>{})>
constexpr auto cell_type(T&&) {
  if
    constexpr(geometry::AABB<T, 3>{} or geometry::Box<T, 3>{}) {
      return hexahedron{};
    }
  else {
    return polyhedron{};
  }
}

template <typename T>
using cell_t = decltype(cell_type(std::declval<T>()));

template <typename T>
using cell_write_type_t = typename uncvref_t<cell_t<T>>::cell_type;

template <typename T>
using to_vtk_cell_t = typename cell_t<T>::type;
template <typename T>
using to_vtk_cell_ptr_t = smart_ptr<to_vtk_cell_t<T>>;

template <typename... Ts>
using vtk_cell_types_t
 = meta::unique<meta::transform<meta::list<Ts...>, meta::quote<to_vtk_cell_t>>>;

template <typename... Ts>
using vtk_cell_ptr_types_t = meta::unique<
 meta::transform<meta::list<Ts...>, meta::quote<to_vtk_cell_ptr_t>>>;

template <typename... Ts>
using vtk_cell_ptr_tuple_t
 = meta::apply<meta::quote<tuple>, vtk_cell_ptr_types_t<Ts...>>;

// TODO: replace std::experimental::variant with just variant
template <typename... Ts>
auto make_tuple_of_cells(variant<Ts...> const&) -> vtk_cell_ptr_tuple_t<Ts...> {
  vtk_cell_ptr_tuple_t<Ts...> r;
  ranges::tuple_for_each(r,
                         [](auto&& t) { t = uncvref_t<decltype(t)>::New(); });
  return r;
}

using vtk_point_t = ::hm3::array<double, 3>;

template <dim_t Ad>
constexpr auto to_vtk_point(geometry::point<Ad> p) noexcept -> vtk_point_t {
  vtk_point_t vtk_p{{0.0, 0.0, 0.0}};
  for (dim_t d = 0; d < Ad; ++d) { vtk_p[d] = p(d); }
  return vtk_p;
}

template <dim_t Ad>
using geometry_t = geometry::some<Ad>;

/// How to write a vertex-based cell
template <typename G, typename TmpCells, typename PointInserter, typename Cells,
          typename CellTypes>
void write_vertex_based_cell(G&& g, TmpCells& tmp_cells,
                             PointInserter& unique_inserter, Cells& cells,
                             CellTypes& cell_types) {
  using geometry_t = uncvref_t<decltype(g)>;

  auto cell_vertices = geometry::vertices(g);

  auto& tmp_cell = std::get<to_vtk_cell_ptr_t<geometry_t>>(tmp_cells);
  HM3_ASSERT(size(cell_vertices) > 0_u, "cells of zero points not supported");

  tmp_cell->GetPointIds()->Reset();
  // Append vertices to the point set, and store the corner ids:
  for (auto&& cIdx : geometry::vertex.indices(g)) {
    vtkIdType id;
    auto corner_vtk = to_vtk_point(cell_vertices[cIdx]);
    unique_inserter->InsertUniquePoint(corner_vtk.data(), id);
    // TODO: this is faster: tmp_cell->GetPointIds()->SetId(cIdx, id);
    //
    // Maybe instead of just doing Reset above, we should get size(vertices())
    // and initialize the cell points, so that we can use SetId
    //
    // The InsertId function does bound checks, so we might want to use
    // that in Debug mode anyways:
    tmp_cell->GetPointIds()->InsertId(cIdx, id);
  }
  // Insert the cell:
  cells->InsertNextCell(tmp_cell->GetPointIds());
  cell_types.push_back(cell_t<geometry_t>::value());
}

/// How to write a face-based cell
template <typename G, typename TmpCells, typename PointInserter, typename Cells,
          typename CellTypes>
void write_face_based_cell(G&& g, TmpCells&, PointInserter& unique_inserter,
                           Cells& cells, CellTypes& cell_types) {
  using geometry_t = uncvref_t<decltype(g)>;

  auto[vertices, adj] = g.face_vertex_adjacency_matrix();
  small_vector<vtkIdType, 20> vertex_ids;

  HM3_ASSERT(size(vertices) > 0_u, "cells of zero points not supported");

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  for (auto&& v : vertices) {
    auto p = to_vtk_point(v);
    vertex_ids.push_back(vtkIdType());
    unique_inserter->InsertUniquePoint(p.data(), vertex_ids.back());
  }

  /// VTK's polyhedron SetFaces is not undocumented so the following
  /// might break. From deciphering its implementation, this is how it works:
  /// - it accepts an array of vtkIdType
  /// - the first id is the number of faces
  /// - for each face:
  ///   - the next id is the number of vertices in the faces
  ///   - the vertex ids follow
  ///
  /// TODO: re-check if a less retarded way to add faces is added to VTK at some
  /// point.
  ///
  /// TODO: re-check if we can at-least insert this as a flat array instead of
  /// as a vtkIdList...

  // face_stuff = [no_faces, face0len, face0v0, ..., face0vlen-1, face1len,
  // ....]
  auto faces = vtkSmartPointer<vtkIdList>::New();
  // add #of faces:
  faces->InsertNextId(vtkIdType(ranges::size(adj)));
  for (auto&& f : adj) {
    // add #of vertices in the face:
    faces->InsertNextId(vtkIdType(ranges::size(f)));
    // add the vertex ids:
    for (auto&& i : f) { faces->InsertNextId(vtkIdType(vertex_ids[i])); }
  }
  cells->InsertNextCell(faces);
  cell_types.push_back(cell_t<geometry_t>::value());
}

}  // namespace vtk
}  // namespace hm3::vis
#endif  // HM3_ENABLE_VTK
