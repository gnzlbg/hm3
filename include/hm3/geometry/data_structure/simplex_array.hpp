#pragma once
/// \file
///
/// Collection of simplices with unique vertices.
///
/// TODO: should be implemented as a half-edge data-structure instead of with a
/// table.
#include <hm3/geometry/data_structure/point_set.hpp>
#include <hm3/geometry/primitive/simplex.hpp>

namespace hm3::geometry {

/// Stores a collection of simplices with unique vertices
///
/// \tparam Nd number of spatial dimensions.
template <dim_t Nd>
struct simplex_array {
  using point_t   = point<Nd>;
  using simplex_t = simplex<Nd>;
  using point_set = point_set<Nd>;
  using box_t     = box<Nd>;
  using aabb_t    = aabb<Nd>;

  static constexpr dim_t no_face_vertices() noexcept { return Nd; }

  /// Vertex index:
  using vidx_t = typename point_set::pidx_t;
  /// Face index:
  using fidx_t = suint_t;

  /// \name Data
  ///@{

  /// Mesh vertices
  point_set vertices_;

  /// Face vertices
  struct face_t {
    array<vidx_t, no_face_vertices()> vertices;
  };
  vector<face_t> faces_;

  simplex_array() = default;

  template <typename SimplexRange>
  simplex_array(SimplexRange const& simplices) {
    const auto no_simplices = ranges::distance(simplices);

    // Allocate simplices in the mesh
    faces_.reserve(no_simplices);  // allocate faces

    // compute box bounding box over all simplices: O(N)
    auto aabb = bounding_volume.aabb(simplices);

    // Allocate vertices (all points are in the bounding box)
    auto bbox = bounding_volume.box(aabb);
    vertices_ = point_set(no_simplices * Nd, bbox);

    // insert simplices:
    for (auto const& s : simplices) {  // ~O(N logN)
      face_t f;
      for (dim_t vx = 0; vx < no_face_vertices(); ++vx) {
        f.vertices[vx] = vertices_.push_back(vertex(s, vx));  // O(logN)
      }
      faces_.push_back(f);
    }
  }

  /// Bounding box of the simplices
  constexpr box_t bounding_box() const noexcept {
    return vertices_.bounding_box();
  }

  /// Geometry of the face at face index \p i
  constexpr simplex_t face(fidx_t i) const noexcept {
    simplex_t s;
    const auto f = faces_[i];
    for (dim_t vx = 0; vx < no_face_vertices(); ++vx) {
      s.vertices[vx] = vertices_.vertex(f[vx]);
    }
    return s;
  }

  /// Number of faces in the mesh
  fidx_t face_size() const noexcept { return faces_.size(); }
  /// Number of vertices in the mesh
  fidx_t vertex_size() const noexcept { return vertices_.size(); }
  /// Range of face indices in the mesh
  auto face_ids() const noexcept { return view::iota(fidx_t(0), face_size()); }
  /// Range of vertex indices in the mesh
  auto vertex_ids() const noexcept {
    return view::iota(vidx_t(0), vertex_size());
  }

  /// Range of faces in the mesh
  auto faces() const noexcept {
    return face_ids() | view::transform([&](auto&& f) { return face(f); });
  }
  /// Range of vertices in the mesh
  auto vertices() const noexcept { return vertices_.points(); }
};

}  // namespace hm3::geometry
