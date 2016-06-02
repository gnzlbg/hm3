#pragma once
/// \file
///
/// Mesh of simplices
#include <hm3/geometry/point_set.hpp>
#include <hm3/geometry/simplex/simplex.hpp>

namespace hm3 {
namespace geometry {

/// Mesh of simplices (points, line segments, triangles)
///
/// TODO: maybe specialize this for 1D...?
template <dim_t Nd>  //
struct simplex_mesh {
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

  /// Faces
  struct face {
    array<vidx_t, Nd> vertices;
  };
  std::vector<face> faces_;

  template <typename SimplexRange>  //
  simplex_mesh(SimplexRange const& simplices) {
    const auto no_simplices = distance(simplices);

    faces_.reserve(no_simplices);  // allocate faces

    // compute box bounding box over all simplices: O(N)
    point_t min = point_t::constant(std::numeric_limits<num_t>::max());
    point_t max = point_t::constant(std::numeric_limits<num_t>::lowest());

    for (auto const& s : simplices) {
      for (auto const& v : s.vertices) {
        for (dim_t d = 0; d < Nd; ++d) {
          min(d) = std::min(min(d), v(d));
          max(d) = std::max(max(d), v(d));
        }
      }
    }

    // Allocate vertices (all points are in the bounding box)
    box_t bounding_box(min, max);
    vertices_ = point_set(no_simplices * Nd, bounding_box);

    // insert simplices:
    for (auto const& s : simplices) {  // O(N logN)
      face f;
      for (dim_t d = 0; d < Nd; ++d) {
        f[d] = vertices_.push_back(s.vertices[d]);  // O(logN)
      }
      faces_.push_back(f);
    }
  }

  constexpr box_t bounding_box() const noexcept {
    return vertices_.bounding_box();
  }

  constexpr simplex_t simplex(fidx_t i) const noexcept {
    simplex_t s;
    auto f = faces_[i];
    for (dim_t d = 0; d < Nd; ++d) { s.vertices[d] = vertices_.vertex(f[d]); }
    return s;
  }
};

}  // namespace geometry
}  // namespace hm3
