#pragma once
/// \file
///
/// A watertight polyhedron.
#include <hm3/geometry/algorithm/approx/segment.hpp>
#include <hm3/geometry/algorithm/direction/segment.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/utility/small_vector.hpp>

namespace hm3::geometry {

namespace polyhedron_primitive {

/// Are two edges "absolutely" equal:
struct edge_abs_eq_fn {
  template <typename T, typename U>
  constexpr bool operator()(T&& t, U&& u) const noexcept {
    if (approx_segment(t, u, 0., 0.)) { return true; }
    auto iu = direction_segment.invert(u);
    return approx_segment(t, iu, 0., 0.);
  }
};

namespace {
constexpr auto const& edge_abs_eq = static_const<edge_abs_eq_fn>::value;
}

/// Watertight Polyhedron.
struct polyhedron {
  using geometry_type = trait::polyhedron;

  using face_value_type  = polygon<3>;
  using edge_value_type  = associated::edge_t<face_value_type>;
  using point_value_type = associated::point_t<edge_value_type>;

  /// The faces of the polyhedron are stored as polygons:
  static constexpr inline suint_t no_inline_polygons = 8;
  static constexpr inline suint_t no_inline_edges    = 14;
  static constexpr inline suint_t no_inline_points   = 10;
  using face_storage_t = small_vector<face_value_type, no_inline_polygons>;
  face_storage_t faces_;

  constexpr dim_t face_size() const noexcept { return faces_.size(); }
  constexpr auto const& faces() const noexcept { return faces_; }
  constexpr auto const& face(dim_t i) const noexcept {
    HM3_ASSERT(i < face_size(), "face idx: {} is out-of-bounds [0, {})", i,
               face_size());
    return ranges::at(faces_, i);
  }

  auto edges() const noexcept {
    small_vector<edge_value_type, no_inline_edges> edges_;
    auto upb = [&](auto&& v) { unique_push_back(edges_, v, edge_abs_eq); };
    for (auto&& f : faces()) {
      for (auto&& e : geometry::edges(f)) { upb(e); }
    }
    return edges_;
  }

  auto edge_size() const noexcept { return edges().size(); }

  auto vertices() const noexcept {
    small_vector<point_value_type, no_inline_points> points_;
    auto upb = [&](auto&& v) { unique_push_back(points_, v, approx_point); };
    for (auto&& f : faces()) {
      for (auto&& p : geometry::vertices(f)) { upb(p); }
    }
    return points_;
  }

  auto vertex_size() const noexcept { return vertices().size(); }

  polyhedron()                  = default;
  polyhedron(polyhedron const&) = default;
  polyhedron(polyhedron&&)      = default;
  polyhedron& operator=(polyhedron const&) = default;
  polyhedron& operator=(polyhedron&&) = default;
  ~polyhedron()                       = default;

  constexpr auto reserve(suint_t no_faces) noexcept {
    faces_.reserve(no_faces);
  }

  /// Construct the face-vertex adjancency matrix of the polyhedron, that is,
  /// for each "global" face, which are the "global"" vertex indices.
  struct face_vertex_adjacency_t {
    using face_vertices_t = small_vector<suint_t, no_inline_points>;
    using adjacency_t     = small_vector<face_vertices_t, no_inline_polygons>;
    adjacency_t data_;

    template <typename T = face_vertices_t>
    void push_back(T&& t = T{}) {
      data_.push_back(std::forward<T>(t));
    }

    auto begin() noexcept { return data_.begin(); }
    auto begin() const noexcept { return data_.begin(); }
    auto end() noexcept { return data_.end(); }
    auto end() const noexcept { return data_.end(); }
    auto size() const noexcept { return data_.size(); }

    auto& operator[](uint_t i) noexcept {
      HM3_ASSERT(i < size(), "index {} is out-of-bounds [0, {})", i, size());
      return ranges::at(data_, i);
    }
    auto const& operator[](uint_t i) const noexcept {
      HM3_ASSERT(i < size(), "index {} is out-of-bounds [0, {})", i, size());
      return ranges::at(data_, i);
    }

    template <typename OStream>
    friend OStream& operator<<(OStream& os,
                               face_vertex_adjacency_t const& t) noexcept {
      suint_t eidx = 0;
      os << "{ polyhedron face-vertex adjacency matrix:\n";
      for (auto&& e : t.data_) {
        os << "  edge: " << eidx << "{\n";
        for (auto&& a : e) { os << "    " << a << "\n"; }
        os << "  }\n";
        ++eidx;
      }
      os << "}\n";
      return os;
    }
  };

  auto face_vertex_adjacency_matrix() const noexcept {
    /// Get the global vertices:
    auto vs = vertices();

    auto vertex_idx = [&](auto&& v) {
      auto it
       = ranges::find_if(vs, [&](auto&& i) { return approx_point(i, v); });
      HM3_ASSERT(it != ranges::end(vs), "all points must be there??");
      return suint_t(it - ranges::begin(vs));
    };

    face_vertex_adjacency_t adjacency;

    for (auto&& f : faces()) {
      adjacency.push_back();
      auto&& last = ranges::back(adjacency);
      for (auto&& v : geometry::vertices(f)) {
        auto idx = vertex_idx(v);
        last.push_back(idx);
      }
    }
    return std::make_pair(vs, adjacency);
  }

  struct edge_face_t {
    using face_idx_t      = suint_t;
    using face_edge_idx_t = suint_t;
    face_idx_t face_idx;
    face_edge_idx_t face_edge_idx;
    template <typename OStream>
    friend OStream& operator<<(OStream& os, edge_face_t const& t) noexcept {
      os << "{face: " << t.face_idx << ", edge: " << t.face_edge_idx << "}";
      return os;
    }
  };

  struct edge_face_adjacency_t {
    using edge_faces_t = small_vector<edge_face_t, 2>;
    using adjacency_t  = small_vector<edge_faces_t, no_inline_edges>;
    adjacency_t data_;

    template <typename T>
    void push_back(T&& t) {
      data_.push_back(std::forward<T>(t));
    }

    auto begin() noexcept { return data_.begin(); }
    auto begin() const noexcept { return data_.begin(); }
    auto end() noexcept { return data_.end(); }
    auto end() const noexcept { return data_.end(); }
    auto size() const noexcept { return data_.size(); }

    auto& operator[](uint_t i) noexcept {
      HM3_ASSERT(i < size(), "index {} is out-of-bounds [0, {})", i, size());
      return ranges::at(data_, i);
    }
    auto const& operator[](uint_t i) const noexcept {
      HM3_ASSERT(i < size(), "index {} is out-of-bounds [0, {})", i, size());
      return ranges::at(data_, i);
    }

    template <typename OStream>
    friend OStream& operator<<(OStream& os,
                               edge_face_adjacency_t const& t) noexcept {
      suint_t eidx = 0;
      os << "{ polyhedron edge-face adjacency matrix:\n";
      for (auto&& e : t.data_) {
        os << "  edge: " << eidx << "{\n";
        for (auto&& a : e) { os << "    " << a << "\n"; }
        os << "  }\n";
        ++eidx;
      }
      os << "}\n";
      return os;
    }
  };

  /// Constructs the edge-face adjancency matrix of the polyhedron, that is, for
  /// each "global" edge, which are the neighbor "global" faces.
  /// It also stores the face "local" indices of the edges.
  ///
  /// TODO: refactor to its own algorithm
  ///
  auto edge_face_adjacency_matrix() const noexcept {
    /// For each edge, stores the two indices of the adjacent polygons:
    /// edge = {left, right};
    /// the left's polygon edge is in the same direction as the edge
    ///
    edge_face_adjacency_t edge_face_adjacency_matrix;

    auto append_edge = [&](auto&& e, edge_face_t new_ef) {
      using edge_faces_t = typename edge_face_adjacency_t::edge_faces_t;
      auto ef_it
       = ranges::find_if(edge_face_adjacency_matrix, [&](auto&& edge_faces) {
           HM3_ASSERT(not edge_faces.empty(), "??");
           // get the first face (all faces should be equal anyways):
           auto ef = edge_faces[0];
           // get the face segment
           auto s = edge(faces_[ef.face_idx], ef.face_edge_idx);
           // check if they are "abs-equal":
           return edge_abs_eq(s, e);
         });

      // if not found create a new entry for this edge and we are done:
      if (ef_it == ranges::end(edge_face_adjacency_matrix)) {
        edge_face_adjacency_matrix.push_back(edge_faces_t{new_ef});
        return;
      }

      // otherwise, append to the existing adjacency entry
      auto& existing_ef = *ef_it;
      existing_ef.push_back(new_ef);
    };

    suint_t face_idx = 0;
    for (auto&& f : faces()) {
      suint_t face_edge_idx = 0;
      for (auto&& e : geometry::edges(f)) {
        append_edge(e, edge_face_t{face_idx, face_edge_idx});
        ++face_edge_idx;
      }
      ++face_idx;
    }
    return edge_face_adjacency_matrix;
  }

  template <typename At>
  void assert_invariants(At&& at) const {
    /// At least 4 faces
    HM3_ASSERT_AT(face_size() >= 4,
                  "watertight polyhedron requires at least 4 faces", at);
    /// Is watertight? If each edge has two different adjacent faces only:
    auto edge_adjacency = edge_face_adjacency_matrix();

    auto max_faces_sharing_edge = ranges::accumulate(
     edge_adjacency, std::size_t{0},
     [](auto&& acc, auto&& e) { return math::max(e.size(), acc); });
    HM3_ASSERT_AT(max_faces_sharing_edge == 2,
                  "The polyhedron is not "
                  "watertight!\n\npolyhedron: "
                  "{}\n\nedge_face_adjacency_matrix: {}\n",
                  at, *this, edge_adjacency);
  }

  template <
   typename Faces, typename FT = ranges::range_value_t<uncvref_t<Faces>>,
   CONCEPT_REQUIRES_(Range<Faces>{} and Constructible<face_value_type, FT>{})>
  explicit polyhedron(Faces&& fs) noexcept {
    for (auto&& f : fs) { faces_.push_back(f); }
    assert_invariants(HM3_AT_);
  }

  template <
   typename Faces, typename FT = ranges::range_value_t<uncvref_t<Faces>>,
   CONCEPT_REQUIRES_(Range<Faces>{} and Constructible<face_value_type, FT>{})>
  polyhedron& operator=(Faces&& fs) noexcept {
    faces_.clear();
    for (auto&& f : fs) { faces_.push_back(f); }
    assert_invariants(HM3_AT_);
    return (*this);
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

inline bool operator==(polyhedron const& a, polyhedron const& b) noexcept {
  return equal(a.faces(), b.faces());
}

inline bool operator!=(polyhedron const& a, polyhedron const& b) noexcept {
  return !(a == b);
}

/// \name VertexAccess
///@{

/// Number of vertices of the polyhedron \p p.
constexpr auto vertex_size(polyhedron const& p) noexcept {
  return p.vertex_size();
}

/// Vertex \p i of the polyhedron \p p.

constexpr auto vertex(polyhedron const& p, dim_t i) noexcept {
  HM3_ASSERT(i < p.vertex_size(), "");
  /// TODO: overload vertices
  // HM3_ASSERT(false,
  //            "don't do this: call vertices(), store the result, and index
  //            it");
  return p.vertices()[i];
}

///@} // VertexAccess

/// \name EdgeAccess
///@{

/// Number of edges in the polyhedron \p p.

constexpr auto edge_size(polyhedron const& p) noexcept { return p.edge_size(); }

/// Edge \p e of the polyhedron \p p.

constexpr auto edge(polyhedron const& p, dim_t e) noexcept {
  HM3_ASSERT(e < p.edge_size(), "");
  /// TODO: overload edges
  // HM3_ASSERT(false,
  //            "don't do this: call edges(), store the result, and index it");
  return p.edges()[e];
}

///@}  // EdgeAccess

/// \name FaceAccess
///@{

/// Number of faces in the polyhedron \p p.

constexpr auto face_size(polyhedron const& p) noexcept { return p.face_size(); }

/// Face \p e of the polyhedron \p p.

inline auto face(polyhedron const& p, dim_t e) noexcept {
  HM3_ASSERT(e < p.face_size(), "");
  return p.face(e);
}

///@}  // FaceAccess

}  // namespace polyhedron_primitive

using polyhedron_primitive::polyhedron;

}  // namespace hm3::geometry
