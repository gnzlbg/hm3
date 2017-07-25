#ifdef ABC
#pragma once
/// \file
///
/// STL surface mesh
#include <hm3/grid/hierarchical/grid.hpp>

namespace hm3 {
namespace grid {
namespace surface {

/// STL surface mesh
///
/// \tparam Ad number of spatial dimensions
/// \tparam MaxTrisPerLeaf number of triangles per leaf bounding volume
///
/// It is implemented as a Bounding Volume Hierarchy (BVH) in which the
/// leaf volumes contain at most `MaxTrisPerLeaf` triangles per leaf.
///
template <suint_t Ad, suint_t NTrisPerLeaf = 4>
struct stl {
  static constexpr suint_t max_tpl = MaxTrisPerLeaf;

  /// Vertex index:
  using vidx_t = suint_t;
  /// Triangle index:
  using tidx_t = suint_t;

  /// \name Data
  ///@{

  /// Point type:
  using p_t = geometry::point<ad>;
  /// Vertex coordinates
  vector<p_t> vertex_coordinates_;

  /// Vertex triangles:
  using vtids_t = stack_vector<tidx_t, max_tpv>;

  /// Triangles sharing vertex:
  vector<vtids_t> vertex_triangles_;

  /// Triangle vertices:
  using tvids_t = std::array<vidx_t, ad>;
  /// Triangle vertices
  vector<tvids_t> triangles_vertices_;

  /// Triangles in node:
  using ntids_t = vector<tidx_t>;
  /// Triangles entirely contained in each grid node:
  boost::flat_unordered_map<grid_node_idx, ntids_t> triangles_in_node_;

  /// Grid
  using g_t = hierarchical::cm<Ad>;
  g_t* g_;
  gidx_t gidx_;

  ///@}  Data

  g_t& grid() noexcept { return *g_; }
  g_t const& grid() const noexcept { return *g_; }
  p_t const& x(vidx_t v) const noexcept { return vertex_coordinates_[v]; }
  vtids_t const& triangles(vidx_t v) const noexcept {
    return triangle_vertices_[t];
  }
  tvids_t const& vertices(tidx_t t) const noexcept {
    return triangle_vertices_[t];
  }
  ntids_t const& triangles(grid_node_idx n) const noexcept {
    return triangles_in_node_[n];
  }

  enum class location { inside, outside, intersected };

  location loc(box_t s) const noexcept {}

  bool is_intersected(box_t s) const noexcept {}

  /// Clip AABB
  clip_polygon_t clip(box_t s) const noexcept {
    static_assert(clip_polygon_t::max_no_points() >= 9, "");

    // find smallest node containing s -> n
    //   from n to root -> m
    //     for each triangle tri in m
    //       clip(tri, m) -> pol
    //       if(!empty(pol)) => append to result
  }

  /// Finds vertex in the grid
  ///
  /// Complexity: O(logN * M) where M is the maximum number of triangles per
  /// node
  ///
  /// \todo MaxTrianglesPerNode is unbounded =/
  ///
  template <typename Vertex>
  vidx_t find_vertex(Vertex&& vx) const noexcept {
    // Leaf node containing vertex:
    auto n = grid().leaf_containing(vx);

    // do {
    //   for each triangle tri in node n {
    //     for each vertex tri_vx in triangle tri {
    //       if (ranges::distance(vx, tri_vx) < vertex_epsilon) {
    //         return tri_vx;  // found !
    //       }
    //     }
    //   }
    //   n = grid().parent(n);
    // } while(n);
    // return invalid();
  }

  /// Insert a vertex and return its index (doesn't modify triangles)
  template <typename Vertex>
  vidx_t push_vertex(Vertex&& vx) noexcept {
    vidx_t vxidx = find_vertex(vx);
    if (!vxidx) {  // TODO: make atomic
      vxidx = vertex_.size();
      vertex_.emplace_back(std::forward<Vertex>(vx));
      HM3_ASSERT(vertex_triangles_.size() == vxidx, "");
      vertex_triangles_.push_back();
    }
    return vxidx;
  }

  template <typename Triangle>
  void push(Triangle&& tri) noexcept {
    // The bounding box must contain the triangle, otherwise error:
    nidx_t n = grid().root();
    if (!geometry::intersection.test(grid.bounding_box(), tri)) {
      HM3_FATAL_ERROR("triangle {} is outside the grid bounding box {}", tri,
                      grid().bounding_box());
    }

    // Find the smallest node fully containing the triangle:
    while () {
      // If any of the children contains the triangle, recurse on that child:
      for (auto cn : grid().children(n)) {
        if (geometry::intersection.test(cg, grid().geometry(cn))) {
          // TODO: assert that the other children do not contain the triangle
          n = cn;
          continue;
        }
      }
      // n is the largest node containing the triangle and we are done:
      break;
    }

    // Create a triangle without vertices
    auto tidx = [&]() {  // TODO: make atomic
      tidx_t tidx= triangle_vertices_.size();
      triangle_vertices_.push_back();
      return tidx;
    }();

    /// Add vertex to the grid and assign the triangle the vertex indices
    triangle_vertices_[tidx] = vertices(tri) | view::transform([&](auto vx) {
                                 return push_vertex(vx);
                               });

    /// Add the triangle to the vertex indices:
    for (auto vidx : triangle_vertices_[tidx]) {
      vertex_triangles_[vidx].push_back(tidx);
    }
  }
};

}  // namespace surface
}  // namespace grid
}  // namespace hm3
#endif  // ABC
