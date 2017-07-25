#pragma once
/// \file
///
/// Discrete solver boundary.
#include <hm3/solver/geometry/surface/boundary.hpp>
#include <hm3/solver/geometry/types.hpp>
#include <hm3/utility/ascii.hpp>
#include <hm3/utility/flat_set.hpp>

// TODO: refactor tree generation for a domain into its own file
#include <hm3/grid/hierarchical/generation/generic.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/node_level.hpp>
#include <hm3/grid/hierarchical/types.hpp>

namespace hm3::solver::geometry {

/// \name Random number utilities
///@{

/// Random float in range [begin, end):
template <typename T>
static num_t random_float(T random_gen, num_t begin, num_t end) noexcept {
  std::uniform_real_distribution<> dis(begin, end);
  return dis(random_gen);
}

/// Random direction.
template <dim_t Ad, typename T>
static hg::vec<Ad> random_vector(T random_gen) noexcept {
  using vec_t = hg::vec<Ad>;
  if constexpr (Ad == 1) {
    // "random" as in I threw a dice once and this is what came out ;)
    return vec_t::unit(0);
  } else if constexpr (Ad == 2) {
    const num_t polar_coordinate
     = random_float(random_gen, num_t{0}, 2. * math::pi);
    return vec_t{
     std::cos(polar_coordinate),  // x
     std::sin(polar_coordinate)   // y
    };
  } else if constexpr (Ad == 3) {
    const num_t azimuthal_coordinate
     = random_float(random_gen, num_t{0}, 2. * math::pi);
    const num_t polar_coordinate = random_float(random_gen, num_t{0}, math::pi);
    return vec_t{
     std::cos(azimuthal_coordinate) * std::sin(polar_coordinate),  // x
     std::sin(azimuthal_coordinate) * std::sin(polar_coordinate),  // y
     std::cos(polar_coordinate)                                    // z
    };
  }
}

template <dim_t Ad, dim_t N>
array<hg::vec<Ad>, N> random_vectors() noexcept {
  std::ranlux24 random_gen{};
  array<hg::vec<Ad>, N> result;
  for (dim_t i = 0; i < N; ++i) { result[i] = random_vector<Ad>(random_gen); }
  return result;
}

///@} // Random number utilities

/// Discrete solver boundary.
///
/// It stores a boundary as a discrete surface mesh of simplices. The boundary
/// is watertight, and encloses a volume.
///
/// \tparam Ad Number of spatial dimensions.
template <dim_t Ad>
struct discrete_domain : hg::with_ambient_dimension<Ad> {
  static_assert(Ad >= 1 and Ad <= 3);

  /// \name Data
  ///@{

  using point_t   = hg::point<Ad>;
  using segment_t = hg::segment<Ad>;
  using ray_t     = hg::ray<Ad>;
  using vec_t     = hg::vec<Ad>;
  using surface_t = boundary_surface<Ad>;
  static_assert(hg::Simplex<surface_t, Ad>{});
  using simplex_t = hg::associated::simplex_t<surface_t>;
  static_assert(hg::Simplex<simplex_t, Ad>{});
  struct empty_t {};

  using edge_t
   = meta::if_c<Ad == 3, segment_t, meta::if_c<Ad == 2, point_t, empty_t>>;

  /// Unique boundary vertices:
  using vertex_set = hg::point_set<point_t>;
  vertex_set unique_vertices_;

  /// Boundary surface data (boundary data):
  using surface_data_t = boundary_idx_t;
  vector<surface_data_t> surface_data_;

  /// Surface index type.
  using sidx_t = hg::bvh::simplex_idx;
  /// Vertex index type.
  using vidx_t = optional_idx<uint_t, struct vertex_idx_t>;
  /// Edge index type (unavailable in 1D, == vidx_t in 2D, and its own type in
  /// 3D).
  using eidx_t = meta::if_c<Ad == 3, optional_idx<uint_t, struct edge_idx_t>,
                            meta::if_c<Ad == 2, vidx_t, empty_t>>;

  /// surface -> vertices adjacency matrix
  using surface_vertices_t = array<vidx_t, Ad>;
  vector<surface_vertices_t> surface_vertices_;

  /// vertices -> surfaces adjacency matrix
  ///
  /// In 1/2D each vertex can have at most 1/2 surfaces
  /// In 3D a vertex can have many adjacent surfaces (~7 is a good average)
  using vertex_surfaces_t
   = meta::if_c<(Ad < 3), fixed_capacity_vector<sidx_t, Ad>,
                small_vector<sidx_t, 7>>;
  vector<vertex_surfaces_t> vertex_surfaces_;

  /// surface -> edges adjacency matrix
  ///
  /// 1D surfaces have no edges, 2D surfaces edges are points (so that no extra
  /// adjacency matrix is needed), 3D surfaces have 3 edges each
  using surface_edges_t = array<eidx_t, Ad == 1 or Ad == 2 ? 0 : 3>;
  using surface_edges_storage_t
   = meta::if_c<Ad == 3, vector<surface_edges_t>, empty_t>;
  surface_edges_storage_t surface_edges_;

  /// edge ->  surface adjacency matrix
  ///
  /// 1D surfaces have no edges, 2D surfaces edges are points (so that no extra
  /// adjacency matrix is needed), in 3D each edge has exactly two surfaces
  /// (otherwise the mesh is not water tight).
  using edge_surfaces_t = fixed_capacity_vector<sidx_t, Ad == 1 ? 0 : 2>;
  using edge_surface_storage_t
   = meta::if_c<Ad == 3, vector<edge_surfaces_t>, empty_t>;
  edge_surface_storage_t edge_surfaces_;

  /// Bounding Volume Hierarchy
  hg::bounding_volume_hierarchy<Ad> bvh_;

  /// These are some random direction vectors used as alternative ray-casting
  /// directions. They are pre-computed ahead of time:
  static constexpr dim_t max_no_rvs        = 10;
  array<vec_t, max_no_rvs> random_vectors_ = random_vectors<Ad, max_no_rvs>();

  ///@} // Data

 public:
  /// Number of surfaces.
  uint_t surface_size() const noexcept { return surface_vertices_.size(); }

  /// Number of vertices.
  uint_t vertex_size() const noexcept { return unique_vertices_.size(); }

  /// Number of edges.
  uint_t edge_size() const noexcept {
    if constexpr (Ad == 1) {
      return 0;
    } else if constexpr (Ad == 2) {
      return vertex_size();
    } else if constexpr (Ad == 3) {
      return edge_surfaces_.size();
    }
  }

  /// Is the boundary mesh empty?
  bool empty() const noexcept { return surface_size() == 0; }

 private:
  /// Insert vertex into the point set and return the global vertex index.
  vidx_t insert_vertex(point_t const& vx) {
    return unique_vertices_.push_back(vx);
  }

  template <typename At>
  static void assert_valid(vidx_t v, At&& at) noexcept {
    HM3_ASSERT_AT(v, "Invalid vertex", std::forward<At>(at));
  }

  template <typename At>
  static void assert_valid(sidx_t v, At&& at) noexcept {
    HM3_ASSERT_AT(v, "Invalid surface", std::forward<At>(at));
  }

  template <typename At, CONCEPT_REQUIRES_(Ad == 3)>
  static void assert_valid(eidx_t v, At&& at) noexcept {
    HM3_ASSERT_AT(v, "Invalid edge", std::forward<At>(at));
  }

  template <typename At>
  static void assert_valid(boundary_idx_t v, At&& at) noexcept {
    HM3_ASSERT_AT(v, "Invalid boundary index", std::forward<At>(at));
  }

 public:
  /// Vertex at index \p v.
  point_t vertex(vidx_t v) const noexcept {
    assert_valid(v, HM3_AT_);
    HM3_ASSERT(*v < unique_vertices_.size(),
               "vertex {} is out-of-range [0, {})", v, unique_vertices_.size());
    return unique_vertices_.point(*v);
  }

  /// Vertex ids.
  auto vertex_ids() const noexcept {
    return view::ints(uint_t{0}, vertex_size())
           | view::transform([](auto&& i) { return vidx_t{i}; });
  }

  /// Vertices.
  auto vertices() const noexcept {
    return vertex_ids() | view::transform([&](auto&& i) { return vertex(i); });
  }

 private:
  /// Ids of the vertices of the surface \p s.
  auto& surface_vertices_ids(sidx_t s) noexcept {
    assert_valid(s, HM3_AT_);
    HM3_ASSERT(*s < surface_vertices_.size(),
               "surface idx {} for accessing the "
               "surface->vertices adjacency is "
               "out-of-bounds [0, {})",
               s, surface_vertices_.size());
    return surface_vertices_[*s];
  }

 public:
  /// Ids of the vertices of the surface \p s.
  auto const& surface_vertices_ids(sidx_t s) const noexcept {
    assert_valid(s, HM3_AT_);
    HM3_ASSERT(*s < surface_vertices_.size(),
               "surface idx {} for accessing the "
               "surface->vertices adjacency is "
               "out-of-bounds [0, {})",
               s, surface_vertices_.size());
    return surface_vertices_[*s];
  }

 private:
  /// Id of the \p lvidx vertex of the surface \p s.
  auto& surface_vertex_id(sidx_t s, suint_t lvidx) noexcept {
    return surface_vertices_ids(s)[lvidx];
  }

 public:
  /// Id of the \p lvidx vertex of the surface \p s.
  auto const& surface_vertex_id(sidx_t s, suint_t lvidx) const noexcept {
    return surface_vertices_ids(s)[lvidx];
  }

  /// Vertex \p lvidx of the surface \p s.
  auto surface_vertex(sidx_t s, suint_t lvidx) const noexcept {
    return vertex(surface_vertex_id(s, lvidx));
  }

  /// Id of the vertex \p vx of surface \p s
  auto surface_vertex_id(sidx_t s, point_t vx) const noexcept {
    for (auto&& vx_id : surface_vertices_ids(s)) {
      if (approx(vertex(vx_id), vx)) { return vx_id; }
    }
    HM3_FATAL_ERROR("surface {} does not contain the vertex {}", s, vx);
  }

  /// Vertices of the surface \p s.
  auto surface_vertices(sidx_t s) const noexcept {
    auto vids = surface_vertices_ids(s);
    using s_t = uncvref_t<decltype(ranges::size(vids))>;
    return view::ints(s_t(0), ranges::size(vids))
           | view::transform([&, vids](auto&& i) { return vertex(vids[i]); });
  }

 private:
  /// Ids of the surfaces that share the vertex \p v.
  auto& vertex_surfaces_ids(vidx_t v) noexcept {
    assert_valid(v, HM3_AT_);
    HM3_ASSERT(*v < vertex_surfaces_.size(),
               "vertex {} out of vertex-surface adjacency range [0, {})", v,
               vertex_surfaces_.size());
    return vertex_surfaces_[*v];
  }

 public:
  /// Ids of the surfaces that share the vertex \p v.
  auto const& vertex_surfaces_ids(vidx_t v) const noexcept {
    assert_valid(v, HM3_AT_);
    HM3_ASSERT(*v < vertex_surfaces_.size(),
               "vertex {} out of vertex-surface adjacency range [0, {})", v,
               vertex_surfaces_.size());
    return vertex_surfaces_[*v];
  }

 private:
  /// Surface data of surface \p s
  auto& surface_data(sidx_t s) noexcept {
    assert_valid(s, HM3_AT_);
    HM3_ASSERT(surface_data_.size() > *s, "");
    return surface_data_[*s];
  }

 public:
  /// Surface data of surface \p s
  auto const& surface_data(sidx_t s) const noexcept {
    assert_valid(s, HM3_AT_);
    HM3_ASSERT(surface_data_.size() > *s, "");
    return surface_data_[*s];
  }

  /// Ids of the surfaces.
  auto surface_ids() const noexcept {
    return view::ints(uint_t{0}, surface_size())
           | view::transform([](auto&& i) { return sidx_t{i}; });
  }

  /// Surface at index \p s
  surface_t surface(sidx_t s) const noexcept {
    if constexpr (Ad == 1) {
      return surface_t{simplex_t{surface_vertex(s, 0)}, surface_data(s)};
    } else if constexpr (Ad == 2) {
      return surface_t{simplex_t{surface_vertex(s, 0), surface_vertex(s, 1)},
                       surface_data(s)};
    } else if constexpr (Ad == 3) {
      return surface_t{simplex_t{hg::make_segment_range(surface_vertices(s))},
                       surface_data(s)};
    }
  }

  /// All surfaces.
  auto surfaces() const noexcept {
    return surface_ids()
           | view::transform([&](auto&& i) { return surface(i); });
  }

  /// Ids of all the edges
  CONCEPT_REQUIRES(Ad > 1)
  auto edge_ids() const noexcept {
    return view::ints(uint_t{0}, edge_size())
           | view::transform([](auto&& i) { return eidx_t{i}; });
  }

  /// Ids of the surfaces sharing the edge \p e.
  CONCEPT_REQUIRES(Ad > 1)
  auto const& edge_surfaces_ids(eidx_t e) const noexcept {
    if constexpr (Ad == 1) {
      return;
    } else if constexpr (Ad == 2) {
      return vertex_surfaces_ids(e);
    } else if constexpr (Ad == 3) {
      assert_valid(e, HM3_AT_);
      HM3_ASSERT(*e < edge_surfaces_.size(), "edge {} is out-of-bounds [0, {})",
                 e, edge_surfaces_.size());
      return edge_surfaces_[*e];
    }
  }

 private:
  CONCEPT_REQUIRES(Ad > 1)
  auto& edge_surfaces_ids(eidx_t e) noexcept {
    if constexpr (Ad == 1) {
      return;
    } else if constexpr (Ad == 2) {
      return vertex_surfaces_ids(e);
    } else if constexpr (Ad == 3) {
      assert_valid(e, HM3_AT_);
      HM3_ASSERT(*e < edge_surfaces_.size(), "edge {} is out-of-bounds [0, {})",
                 e, edge_surfaces_.size());
      return edge_surfaces_[*e];
    }
  }

 public:
  /// Ids of the edges of the surface \p s.
  CONCEPT_REQUIRES(Ad > 1)
  auto const& surface_edges_ids(sidx_t s) const noexcept {
    if constexpr (Ad == 1) {
      return;
    } else if constexpr (Ad == 2) {
      return surface_vertices_ids(s);
    } else if constexpr (Ad == 3) {
      assert_valid(s, HM3_AT_);
      HM3_ASSERT(*s < surface_edges_.size(),
                 "surface {} is out-of-bounds [0, {})", s,
                 surface_edges_.size());
      return surface_edges_[*s];
    }
  }

  /// Ids of the edges of the surface \p s.
  CONCEPT_REQUIRES(Ad > 1)
  auto& surface_edges_ids(sidx_t s) noexcept {
    if constexpr (Ad == 1) {
      return;
    } else if constexpr (Ad == 2) {
      return surface_vertices_ids(s);
    } else if constexpr (Ad == 3) {
      assert_valid(s, HM3_AT_);
      HM3_ASSERT(*s < surface_edges_.size(),
                 "surface {} is out-of-bounds [0, {})", s,
                 surface_edges_.size());
      return surface_edges_[*s];
    }
  }

  /// Id of the \p e -th edge of the surface \p s.
  CONCEPT_REQUIRES(Ad > 1)
  auto& surface_edge_id(sidx_t s, suint_t e) noexcept {
    if constexpr (Ad == 1) {
      return;
    } else if constexpr (Ad == 2) {
      return surface_vertex_id(s, e);
    } else if constexpr (Ad == 3) {
      auto& es = surface_edges_ids(s);
      HM3_ASSERT(e < es.size(), "local edge {} is out-of-bounds [0, {})", e,
                 es.size());
      return es[e];
    }
  }

  /// Edge at index \p e
  CONCEPT_REQUIRES(Ad > 1)
  edge_t edge(eidx_t e) const noexcept {
    if constexpr (Ad == 1) {
      HM3_FATAL_ERROR("1D boundary surfaces have no edges!");
    } else if constexpr (Ad == 2) {
      return edge_t{vertex(*e)};
    } else if constexpr (Ad == 3) {
      auto s  = edge_surfaces_ids(e);
      auto es = surface_edges_ids(s);
      auto it = ranges::find(es, e);
      HM3_ASSERT(it != ranges::end(es), "");
      auto leidx   = it - ranges::begin(es);
      auto x0_lidx = leidx;
      auto x1_lidx = leidx == 2 ? 0 : leidx + 1;
      return edge_t{surface_vertex(s, x0_lidx), surface_vertex(s, x1_lidx)};
    }
  }

  /// All edges.
  CONCEPT_REQUIRES(Ad > 1)
  auto edges() const noexcept {
    edge_ids() | view::transform([&](auto&& e) { return edge(e); });
  }

 private:
  void surface_data_append(sidx_t s, boundary_idx_t d) {
    assert_valid(s, HM3_AT_);
    assert_valid(d, HM3_AT_);
    HM3_ASSERT(surface_data_.size() == *s,
               "the data for the surface {} has "
               "already been appended "
               "(surface_data.size() == {})",
               s, surface_data_.size());
    surface_data_.push_back(d);
  }

  /// Is the surface \p s in the vertex->surface adjacency of \p v?
  bool vertex_surface_adjacency_contains(vidx_t v, sidx_t s) const noexcept {
    assert_valid(v, HM3_AT_);
    assert_valid(s, HM3_AT_);
    if (vertex_surfaces_.size() <= *v) { return false; }
    return ranges::any_of(vertex_surfaces_ids(v),
                          [s](auto&& i) { return i == s; });
  }

  /// Is the vertex->surface adjacency of \p v full?
  ///
  /// In 1/2D it can contain at most 2 surfaces.
  bool vertex_surface_adjacency_full(vidx_t v) {
    assert_valid(v, HM3_AT_);
    return Ad < 3 ? vertex_surfaces_ids(v).size() >= 2 : false;
  }

  /// Append surface \p s to the vertex->surface adjacency of \p v.
  void vertex_surface_adjacency_append(vidx_t v, sidx_t s) noexcept {
    assert_valid(s, HM3_AT_);
    assert_valid(v, HM3_AT_);
    HM3_ASSERT(
     not vertex_surface_adjacency_contains(v, s),
     "The surface {} is already in the vertex->surface adjacency of vertex {}",
     s, v);
    if (vertex_surfaces_.size() == *v) { vertex_surfaces_.push_back({}); }
    // ascii_fmt::utility_detail::range_to_ascii_impl(
    //  std::cout, vertex_surfaces_ids(v) | view::transform([&](auto&& i) {
    //               return make_pair(i, surface(i));
    //             }));
    // ascii_fmt::out("\n");
    HM3_ASSERT(not vertex_surface_adjacency_full(v),
               "The vertex->surface adjacency of vertex {} is full!\n"
               "  - when trying to append surface {}!\n"
               "  - Note: in 1D and 2D a vertex can be shared"
               " by at most two surfaces (current ambient dim is {})!\n"
               "  - It already contains the surface ids: {}\n\n{}",
               v, s, Ad, vertex_surfaces_ids(v),
               vertex_surfaces_ids(v) | view::transform([&](auto&& i) {
                 return make_pair(i, surface(i));
               }));
    vertex_surfaces_ids(v).push_back(s);
  }

  /// Insert surface \p surface into the boundary mesh.
  ///
  /// \pre Surface data of \p s must be set.
  void insert(surface_t surface) {
    // Global surface id:
    sidx_t sidx = surface_size();

    {  // Insert vertices in the point set and get global vertex ids:
      surface_vertices_t vxs;
      for (auto&& i : hg::vertex.indices(surface)) {
        vxs[i] = insert_vertex(hg::vertex(surface, i));
      }
      HM3_ASSERT(ranges::all_of(vxs, [](auto&& i) { return i.has_value(); }),
                 "invalid vertex ids!");
      // Set the surface's global vertex ids:
      surface_vertices_.push_back(vxs);
    }

    // Insert the surface data:
    HM3_ASSERT(surface.id.has_value(), "surface data not set");
    surface_data_append(sidx, surface.id);

    // Add surface index to the vertex->surfaces adjacency of its vertices:
    for (auto&& v : surface_vertices_ids(sidx)) {
      vertex_surface_adjacency_append(v, sidx);
    }

    // The vertex <---> surface adjacency is now updated!
    // note: asserting that the adjacency matrix is consistent here is too
    // expensive. This is checked in its valid.

    // Update edge <---> surface adjacency matrix:

    // In 1D and 2D nothing to do here:
    // - 1D points have no edges
    // - 2D segments have points as edges, so the surface<--->vertex and
    // - surface<--->edge are equal

    if constexpr (Ad == 3) {
      // Range of edges as vertex ids pairs:
      auto edges = view::ints(0, 3) | view::transform([&](auto&& i) {
                     auto x0= surface_vertex_id(sidx, i);
                     auto x1= surface_vertex_id(sidx, i == 2 ? 0 : i + 1);
                     return std::make_pair(x0, x1);
                   });
      // Finds the local edge idx of surface \p s containing global vertex
      // indices \p x0, \p x1.
      auto find_edge = [&](auto x0, auto x1, auto s) {
        assert_valid(x0, HM3_AT_);
        assert_valid(x1, HM3_AT_);
        HM3_ASSERT(x0 != x1,
                   "cannot find edge for 2 equal vertices with index {}");
        auto const& vs = surface_vertices_ids(s);
        auto it0       = ranges::find(vs, x0);
        auto it1       = ranges::find(vs, x1);
        HM3_ASSERT(it0 != ranges::end(vs), "");
        HM3_ASSERT(it1 != ranges::end(vs), "");
        return it0 > it1 ? it1 - ranges::begin(vs) : it0 - ranges::begin(vs);
      };

      auto edge_surface_append = [&](auto&& e, auto&& s) {
        assert_valid(e, HM3_AT_);
        assert_valid(s, HM3_AT_);
        auto& es = edge_surfaces_ids(e);
        HM3_ASSERT(not es.full(), "edge->surface adjacency of edge {} full", e);
        es.push_back(s);
      };

      // Create the surface<--->edges adjacency matrix for sidx
      HM3_ASSERT(surface_edges_.size() == *sidx, "");
      surface_edges_.push_back({});

      /// Finds surface containing vertex:
      auto find_surface_containing_vertex = [&](auto&& surfaces, auto x) {
        for (auto s : surfaces) {
          if (s == sidx) { continue; }

          if (ranges::any_of(surface_vertices_ids(s),
                             [x](auto&& i) { return i == x; })) {
            return s;
          }
        }
        return sidx_t{};
      };

      for (auto e : edges) {
        auto[x0, x1] = e;

        /// Find the adjacent surface that contains both vertices
        auto x0_surfaces = vertex_surfaces_ids(x0);
        auto x1_surfaces = vertex_surfaces_ids(x1);

        auto s = (ranges::size(x0_surfaces) < ranges::size(x1_surfaces))
                  ? find_surface_containing_vertex(x0_surfaces, x1)
                  : find_surface_containing_vertex(x1_surfaces, x0);

        // If there is an adjacent surface, update the adjacency matrix:
        if (s) {
          auto local_eidx = find_edge(x0, x1, s);
          auto eidx       = surface_edge_id(s, local_eidx);
          HM3_ASSERT(
           eidx,
           "surface {} has broken surface->edge adjacency!\n"
           " - when building surface<-->edge adjacency for surface {}\n"
           " - surface {} local edge {} has no entry in the "
           "surface->edge adjacency of surface {}\n"
           " - surface->edge adj: {}\n"
           " - edge {} vertices: {}, {}\n",
           s, sidx, s, local_eidx, s, surface_edges_ids(s), local_eidx,
           make_pair(surface_vertex_id(s, local_eidx),
                     vertex(surface_vertex_id(s, local_eidx))),
           make_pair(surface_vertex_id(s, local_eidx == 2 ? 0 : local_eidx + 1),
                     vertex(surface_vertex_id(
                      s, local_eidx == 2 ? 0 : local_eidx + 1))));
          /// Append sidx to eidx adjancency
          edge_surface_append(eidx, sidx);
          /// Append eidx to surface adjacency
          auto s_local_eidx                   = find_edge(x0, x1, sidx);
          surface_edge_id(sidx, s_local_eidx) = eidx;
          break;
        }
        // Otherwise, there is no other surface sharing the edge [x0, x1] with
        // the surface yet. So we create the global edge:
        auto global_eidx = eidx_t(edge_surfaces_.size());
        edge_surfaces_.push_back({});
        // Append the surface to it:
        edge_surface_append(global_eidx, sidx);

        // And set the local edge to point to it:
        auto local_eidx                   = find_edge(x0, x1, sidx);
        surface_edge_id(sidx, local_eidx) = global_eidx;
      }
    }

    /// edge <---> surface adjacency is now updated
  }

  template <
   typename SurfaceRng,
   CONCEPT_REQUIRES_(
    Range<SurfaceRng>{}
    and Same<uncvref_t<ranges::range_value_t<SurfaceRng>>, surface_t>{})>
  void insert(SurfaceRng&& surfaces) {
    for (auto&& s : surfaces) { insert(s); }
  }

 private:
  /// Is a boundary mesh water tight?
  ///
  /// In 1D a boundary mesh is watertight if it does contain two non-equal
  /// vertices.
  ///
  /// In 2D and 3D a boundary mesh is watertight if every edge is shared by
  /// exactly two surfaces.
  bool is_watertight() const noexcept {
    if constexpr (Ad == 1) {
      return vertex_size() == 2
             and surface_vertices_[0] != surface_vertices_[1];
    }

    if constexpr (Ad > 1) {
      for (auto&& e : edge_ids()) {
        if (ranges::count(edge_surfaces_ids(e), sidx_t{}) != 0) {
          return false;
        }
      }
      return true;
    }

    return false;
  }

  /// Adapts as a BVH mesh
  struct bvh_mesh {
    using simplex_idx = hg::bvh::simplex_idx;
    discrete_domain const& m;
    auto size() const noexcept { return m.surface_size(); }
    auto face_ids() const noexcept {
      return m.surface_ids()
             | view::transform([](auto&& i) { return simplex_idx{*i}; });
    }
    auto simplex(simplex_idx s) const noexcept { return m.surface(sidx_t{*s}); }
  };

  /// Adapts the boundary mesh as a BVH mesh:
  bvh_mesh as_bvh_mesh() const noexcept { return bvh_mesh{*this}; }

  /// Initializes the bounding volume hierarchy.
  void initialize_bvh() { bvh_.rebuild(as_bvh_mesh()); }

 public:
  /// Computes whether a boudnary mesh is valid.
  ///
  /// A boundary mesh is valid if:
  /// - It's watertight (no holes).
  /// - Its adjacency matrices are consistent.
  bool valid() const noexcept {
    // check consistent adjacency matrices
    return is_watertight();
  }

  /// Does the primitive \p p intersect the boundary?
  template <typename P>
  bool intersection_test(P&& p) const noexcept {
    return bvh_.intersection_test(as_bvh_mesh(), p);
  }

  /// Relative position of the point \p p w.r.t the boundary.
  ///
  /// If the point is on a simplex, the point is intersected by the boundary.
  ///
  /// Otherwise, it will be outside or inside.
  ///
  /// This is determined by counting the number of non-degenerate intersections
  /// between a ray with origin at the point and the boundary. When the count is
  /// even, the point is outside, otherwise (when the count is odd) the point is
  /// inside the domain.
  hg::relative_position_t relative_position_point(point_t p) const noexcept {
    // 1. If the point is not inside the bounding box of the aabb, its outside:
    if (not hg::intersection.test(p, bounding_box())) { return hg::outside; }

    // 2. If the point lies on a simplex, it is intersected:
    auto s = bvh_.closest_simplex(as_bvh_mesh(), p);
    if (hg::approx_number(s.d, 0.)) { return hg::intersected; }

    // 3. Not intersected: cast rays and count the number of non-degenerate
    // intersections with the boundary. An odd number of intersections means
    // that the point is inside, an even number of intersections means that the
    // point is outside.

    // 3.1 Cast ray in direction of the closest wall of the BVH bounding box:
    // (to hopefully intersect as few AABBs as possible):
    auto r = ray_t{p, hg::direction.closest(bounding_box(), p)};
    // 3.2 Compute the number of non-degenerate ray simplex intersections:
    auto intersection_count
     = bvh_.non_degenerate_intersection_count(as_bvh_mesh(), r);

    // 4 If a degenerate intersection was found, cast a ray from the point in a
    // random direction, and retry until a non-degenerate intersection count is
    // found:
    suint_t rv = 0;
    while (not intersection_count) {
      ascii_fmt::out("point {} with ray {} degenerate! next ray: {}\n", p, r,
                     ray_t{p, random_vectors_[rv]});
      intersection_count = bvh_.non_degenerate_intersection_count(
       as_bvh_mesh(), ray_t{p, random_vectors_[rv]});
      ++rv;
      HM3_ASSERT(rv < max_no_rvs,
                 "You are either extremely unlucky or your mesh is pure evil! "
                 "Maybe increase discrete_domain::max_no_rvs a bit, but the "
                 "chances that 10 random rays hit boundary surfaces on "
                 "degenerate locations are _really low_ and you should be "
                 "ashamed of your geometry?");
    }

    HM3_ASSERT(intersection_count, "");

    // 5. And even count of intersections means that the point is outside.
    // Otherwise, the point is inside:
    return (*intersection_count) % 2 == 0 ? hg::outside : hg::inside;
  }

  /// Relative position of the polygon \p p w.r.t. the domain.
  template <typename P>
  hg::relative_position_t relative_position_polygon(P&& p) const noexcept {
    using rp_t = hg::relative_position_t;
    static_assert(hg::Polygon<P>{});

    auto vxs = hg::vertices(p);
    // ascii_fmt::out("rp of {} | vertices rp: {}", p,
    //                vxs | view::transform([&](auto&& vx) {
    //                  return relative_position_point(vx);
    //                }));

    const rp_t first_rp = relative_position_point(hg::vertex.first(p));

    // 1. Check all if all vertices have the same relative position.
    //   --> if relative positions differ, the polygon is intersected

    if (ranges::any_of(vxs | view::drop(1) | view::transform([&](auto&& vx) {
                         return relative_position_point(vx);
                       }),
                       [&](auto&& rp) { return rp != first_rp; })) {
      // ascii_fmt::out("  => intersected\n");
      return rp_t::intersected;
    }

    // 2. All vertices have the same relative position:
    //   --> if all vertices are inside/intersected, the polygon is
    //   inside/intersected
    HM3_ASSERT(
     ranges::all_of(
      vxs, [&](auto&& v) { return relative_position_point(v) == first_rp; }),
     "all vertices don't have the same relative position ?!");
    if (first_rp != rp_t::outside) {
      // ascii_fmt::out("  => {}\n", first_rp);
      return first_rp;
    }

    // 3. All vertices are outside: check if the polygon contains the domain
    // (that is, if all points of the domain are inside the polygon):
    //   --> if a point of the domain is inside or on the polygon, the polygon
    //   is intersected
    //   --> if a point of the domain is outside the polygon, the polygon is
    //   outside

    HM3_ASSERT(!unique_vertices_.empty(), "domain has no vertices?");
    const auto v0_rp = hg::relative_position(unique_vertices_.point(0), p);
    if (v0_rp != rp_t::outside) {
      // ascii_fmt::out("  => intersected\n");
      return rp_t::intersected;
    }
    HM3_ASSERT(v0_rp == rp_t::outside, "{} != outside??", v0_rp);
    // ascii_fmt::out("  => outside\n");
    return rp_t::outside;
  }

  /// Relative position of the primitive \p t w.r.t. the boundary.
  template <typename T>
  hg::relative_position_t relative_position(T&& t) const noexcept {
    using UT = uncvref_t<T>;
    static_assert(hg::Point<UT>{} || hg::Polygon<UT>{},
                  "TODO: implement for other primitives");
    if constexpr (hg::Point<UT>{}) {
      return relative_position_point(std::forward<T>(t));
    } else if constexpr (hg::Polygon<UT>{}) {
      return relative_position_polygon(std::forward<T>(t));
    }
    HM3_UNREACHABLE();
  }

  /// Signed-distance from point \p p to the boundary.
  num_t signed_distance(point_t p) const noexcept {
    switch (relative_position(p)) {
      case hg::intersected: {
        return 0.;
      }
      case hg::inside: {
        auto d = bvh_.closest_simplex(as_bvh_mesh(), p);
        HM3_ASSERT(d, "no closest simplex found?");
        return d.value().d;
      }
      case hg::outside: {
        auto d = bvh_.closest_simplex(as_bvh_mesh(), p);
        HM3_ASSERT(d, "no closest simplex found?");
        return -1. * d.value().d;
      }
    }
  }

  /// Simplices intersected by the volume \p v.
  template <typename Volume>
  small_vector<sidx_t, 4> intersected_simplices(Volume&& v) const noexcept {
    static constexpr bool is_aabb = hg::AABB<uncvref_t<Volume>>{};
    // Find leaf nodes in the bvh that intersect the AABB:
    small_vector<hg::bvh::node_idx, 4> intersected_bvh_nodes_;

    // Obtain the aabb of the volume (a no-op if its already an aabb)
    auto a = hg::bounding_volume.aabb(v);
    bvh_.aabb_intersection(a, intersected_bvh_nodes_);

    // Add the simplices in those leaf nodes that intersect the AABB:
    small_vector<sidx_t, 4> intersected_simplices_;
    for (auto&& nidx : intersected_bvh_nodes_) {
      auto n = bvh_.node(nidx);
      HM3_ASSERT(n.simplex, "");
      auto sidx = sidx_t{*n.simplex};
      auto s    = surface(sidx);
      // If the simplex intersect the volume's aabb:
      //  - and the volume is an aabb,
      //  - or the simplex intersect the volume,
      //  then add the simplex.
      if (hg::intersection.test(a, s)
          and (is_aabb or hg::intersection.test(v, s))) {
        intersected_simplices_.push_back(sidx);
      }
    }

    ranges::sort(intersected_simplices_);
    return intersected_simplices_;
  }

  // TODO: intersected simplices for segments/rays

  /// Split the volume \p v with the discrete boundary.
  template <typename Volume>
  auto split(Volume&& v) const noexcept {
    // 1. Find simplices intersected by the volume
    auto is = intersected_simplices(v);

    // 2. Concatenates single polylines/polysurfaces into bigger ones
    auto ps = hg::concatenate.range(
     is | view::transform([&](auto&& sidx) { return surface(sidx); }));

    // 3. Split the volume with the polysurfaces
    return hg::split.against_range(v, ps);
  }

  template <typename Volume>
  auto clip(Volume&& v, hg::relative_position_t rp) const noexcept {
    HM3_ASSERT(rp != hg::intersected,
               "TODO: implement clip volume to a boundary");
    // 1. Split the volume with the boundary
    auto volumes = split(v);

    // 2. Remove the results at the wrong relative position
    //
    // That is, if the relative position of any of its vertices is any of the
    // vertices is not either intersected or in rp
    action::remove_if(volumes, [&](auto&& v_) {
      for (auto&& p : hg::vertices(v_)) {
        auto p_rp = relative_position(p);
        if (p_rp == hg::intersected) { continue; }
        if (p_rp == rp) { return false; }
        return true;
      }
      HM3_FATAL_ERROR("all vertices are intersected ?");
      return false;
    });

    // 3. Return those matching the query
    return volumes;
  }

  /// Find closest surface to point
  auto closest_surface_ids(point_t p) const noexcept {
    auto r = bvh_.closest_simplex(as_bvh_mesh(), p);
    HM3_ASSERT(r.valid(), "??");
    auto sidx = bvh_.simplex(r.n);
    struct {
      sidx_t surface;
      num_t distance;
    } val{*sidx, r.d};
    return val;
  }

  /// Signed distance at point

  /// Bounding AABB
  hg::aabb<Ad> bounding_aabb() const noexcept { return bvh_.bounding_aabb(); }

  /// Bounding box
  hg::box<Ad> bounding_box() const noexcept { return bvh_.bounding_box(); }

  discrete_domain()                       = default;
  discrete_domain(discrete_domain const&) = default;
  discrete_domain(discrete_domain&&)      = default;
  discrete_domain& operator=(discrete_domain const&) = default;
  discrete_domain& operator=(discrete_domain&&) = default;

  template <
   typename SurfaceRng,
   CONCEPT_REQUIRES_(
    Range<SurfaceRng>{}
    and Same<uncvref_t<ranges::range_value_t<SurfaceRng>>, surface_t>{})>
  discrete_domain(SurfaceRng&& surface_rng) {
    /// Insert the surfaces:
    insert(std::forward<SurfaceRng>(surface_rng));
    /// Validate the surface mesh:
    HM3_ASSERT(valid(), "invalid mesh");
    /// Generate the bounding volume hierarchy:
    initialize_bvh();
  }
};

// TODO: refactor tree generation for a discrete domain into its own file
template <typename Tree, typename Domain>
void uniform_refine(Tree& tree, Domain const& domain,
                    tree::level_idx target_level) {
  tree.log("Uniform refine for discrete domain; target level is {}. Start...",
           target_level);
  tree::level_idx level{0};
  grid::hierarchical::generation::generic(
   // which nodes to select
   [&]() {
     tree.log("  Refining level {}...", level);
     return tree.nodes() | tree.leaf() | tree::node_level.filter(tree, level);
   },
   // how to refine the nodes:
   [&](auto n) {
     auto r = tree::balanced_refine(tree, n);
     if (not r) {
       HM3_FATAL_ERROR(
        "Cannot refine the tree node: {} | tree size: {} | tree capacity: {}",
        n, tree.size(), tree.capacity());
     }
   },
   // when to refine a node:
   [&](auto n) {
     // If we are already at the target level, always return false
     // TODO: check if this is hoisted out of the node loop.
     if (level == target_level) { return false; }
     // If the node is completely outside the domain --> do not refine the node
     auto ng = tree.geometry(n);
     if (domain.relative_position(ng) == hg::relative_position_t::outside) {
       return false;
     }

     // Otherwise the node is either inside or intersected and not at the target
     // level --> refine the node
     return true;
   },
   [&]() { level++; });
  tree.log("... Done. Uniform refine for discrete domain finished.");
}

}  // namespace hm3::solver::geometry
#ifdef HM3_ENABLE_VTK
#include <hm3/vis/vtk/serialize.hpp>

namespace hm3::solver::geometry::vtk {

template <typename State>
struct serializable : hg::with_ambient_dimension<ad_v<State>> {
  State const& s;

  using vtk_cell_idx = typename uncvref_t<State>::sidx_t;

  auto geometry(vtk_cell_idx c) const noexcept { return s.surface(c); }

  auto nodes() const noexcept { return s.surface_ids(); }

  auto bounding_box() const noexcept { return s.bounding_box(); }

  template <typename F>
  auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }

  template <typename CellData>
  void load(CellData&& cell_data) const {
    cell_data.load("surface_idx", [&](vtk_cell_idx n, auto&&) {
      return n ? static_cast<int_t>(*n) : int_t{-1};
    });
    cell_data.load("bcidx", [&](vtk_cell_idx c, auto&&) {
      auto bcidx = s.surface_data(c);
      return bcidx ? int_t{*bcidx} : int_t{-1};
    });
  }

  serializable(State const& s_) : s(s_) {}
};

template <typename State>
void serialize(State const& state, string file_name) {
  hm3::log::serial log("solver::discrete_domain-serialization-to-vtk");

  serializable<State> s(state);
  ::hm3::vis::vtk::serialize(s, file_name, log);
}

}  // namespace hm3::solver::geometry::vtk

#endif
