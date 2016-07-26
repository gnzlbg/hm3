#pragma once
/// \file
///
/// Collection of simplices with unique vertices.
///
/// TODO: should be implemented as a half-edge data-structure instead of with a
/// table.
#include <hm3/geometry/discrete/data_structure/point_set.hpp>
#include <hm3/geometry/discrete/primitive/simplex.hpp>

namespace hm3 {
namespace geometry {

namespace discrete {

/// Stores a collection of simplices with unique vertices
///
/// \tparam Nd number of spatial dimensions.
template <dim_t Nd>  //
struct simplex_array {
  using point_t   = point<Nd>;
  using simplex_t = simplex<Nd>;
  using point_set = point_set<Nd>;
  using box_t     = box<Nd>;
  using aabb_t    = aabb<Nd>;

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
    array<vidx_t, Nd> vertices;
  };
  ::hm3::vector<face_t> faces_;

  simplex_array() = default;

  template <typename SimplexRange>  //
  simplex_array(SimplexRange const& simplices) {
    const auto no_simplices = ranges::distance(simplices);

    // Allocate simplices in the mesh
    faces_.reserve(no_simplices);  // allocate faces

    // compute box bounding box over all simplices: O(N)
    point_t min = point_t::constant(math::highest<num_t>);
    point_t max = point_t::constant(math::lowest<num_t>);

    for (auto const& s : simplices) {
      RANGES_FOR (auto const& v, ::hm3::geometry::vertices(s)) {
        for (dim_t d = 0; d < Nd; ++d) {
          min(d) = std::min(min(d), v(d));
          max(d) = std::max(max(d), v(d));
        }
      }
    }

    // Allocate vertices (all points are in the bounding box)
    box_t bounding_box(bounding_volume.box(aabb_t(min, max)));
    vertices_ = point_set(no_simplices * Nd, bounding_box);

    // insert simplices:
    for (auto const& s : simplices) {  // O(N logN)
      face_t f;
      for (dim_t d = 0; d < Nd; ++d) {
        f.vertices[d] = vertices_.push_back(vertex(s, d));  // O(logN)
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
    for (dim_t d = 0; d < Nd; ++d) { s.vertices[d] = vertices_.vertex(f[d]); }
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

}  // namespace discrete

using discrete::simplex_array;

}  // namespace geometry
}  // namespace hm3
