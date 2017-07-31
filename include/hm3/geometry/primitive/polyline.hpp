#pragma once
/// \file
///
/// Polyline.
#include <hm3/geometry/concept/resizable.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/geometry/primitive/data.hpp>
#include <hm3/geometry/primitive/detail/generic_storage.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/io/ascii.hpp>
#include <hm3/utility/small_vector.hpp>

namespace hm3::geometry {

struct point_based_t {};
struct edge_based_t {};

/// Polyline primitive
namespace polyline_primitive {

// Edge type of a polyline. If the storage contains points, the edge type is a
// segment with the point as its point type.
template <typename Storage, typename T = ranges::value_type_t<Storage>>
using edge_t = meta::if_c<Point<T>{}, associated::edge_t<T>,
                          meta::if_c<Edge<T>{}, T, void>>;

template <typename Storage, typename T = ranges::value_type_t<Storage>>
using polyline_type_t = meta::if_c<Point<T>{}, point_based_t,
                                   meta::if_c<Edge<T>{}, edge_based_t, void>>;

/// Polyline.
///
/// \tparam Ad Ambient space dimension.
/// \tparam Storage Vector-like container for storing the polyline points or
/// edges.
template <dim_t Ad, typename Storage, typename Data>
struct polyline : public primitive_data<Data> {
  using storage_type = polyline_type_t<Storage>;
  using self         = polyline<Ad, Storage, Data>;

  static_assert(
   !Same<storage_type, void>{},
   "error: the type stored in the polyline must model either Point or Edge");

  // Storage for the vertices
  using data_t                  = detail::generic_storage<Storage>;
  using geometry_type           = trait::polyline<Ad>;
  using edge_value_type         = edge_t<Storage>;
  using point_value_type        = associated::point_t<edge_value_type>;
  using concatenated_value_type = self;
  data_t data_;

  /// \name Vertices
  ///@{

  /// Number of vertices in the polyline.
  dim_t vertex_size() const {
    if constexpr (Same<storage_type, point_based_t>{}) {
      return ranges::size(data_);
    } else if constexpr (Same<storage_type, edge_based_t>{}) {
      return edge_size() + 1;
    }
  }

  /// Vertex \p i in the polyline.
  ///
  /// If the polyline is edge based, the vertices are the first vertex of each
  /// edge, except for the last vertex in the polyline which is the second
  /// vertex of the last edge.
  point_value_type vertex(dim_t i) const {
    HM3_ASSERT(i < vertex_size(), "vertex index {} is out-of-bounds [0, {})", i,
               vertex_size());
    if constexpr (Same<storage_type, point_based_t>{}) {
      return data_[i];
    } else if constexpr (Same<storage_type, edge_based_t>{}) {
      return i == edge_size() ? data_[i - 1].x(1) : data_[i].x(0);
    }
  }

  /// Range of vertices in the polyline.
  auto vertices() const {
    return view::iota(0, vertex_size())
           | view::transform([&](auto&& i) { return vertex(i); });
  }

  ///@}  // Vertices

  /// \name Edges
  ///@{

  /// Number of edges in the polyline.
  dim_t edge_size() const {
    if (Same<storage_type, point_based_t>{}) {
      return vertex_size() < 2 ? 0 : vertex_size() - 1;
    } else {
      return ranges::size(data_);
    }
  }

  /// Edge \p i of the polyline.
  edge_value_type edge(dim_t i) const {
    HM3_ASSERT(i < edge_size(), "edge index {} is out-of-bounds [0, {})", i,
               edge_size());
    if (Same<storage_type, point_based_t>{}) {
      return edge_value_type(vertex(i), vertex(i + 1));
    } else if constexpr (Same<storage_type, edge_based_t>{}) {
      return data_[i];
    }
  }

  /// Range of edges in the polyline
  auto edges() const {
    return view::iota(0, edge_size())
           | view::transform([&](auto&& i) { return edge(i); });
  }

  // Appends an edge to the polyline (storage is point-based)
  CONCEPT_REQUIRES(Resizable<Storage>{})
  void push_back_edge(edge_value_type const& e, point_based_t) {
    if (data_.empty()) {
      data_.push_back(e.x(0));
    } else {
      HM3_ASSERT(approx_point(data_[vertex_size() - 1], e.x(0)),
                 "Fail to append edge! Last polyline vertex does not match "
                 "first vertex of edge!\n\n  edge: {}\n  polyline: {}\n",
                 e, *this);
    }
    data_.push_back(e.x(1));
  }

  // Appends an edge to the polyline (storage is edge-based)
  CONCEPT_REQUIRES(Resizable<Storage>{})
  void push_back_edge(edge_value_type const& e, edge_based_t) {
    HM3_ASSERT(data_.empty()
                or approx_point(data_[edge_size() - 1].x(1), e.x(0)),
               "Fail to append edge! Last polyline vertex does not match "
               "first vertex of edge!\n\n  edge: {}\n  polyline: {}\n",
               e, *this);
    data_.push_back(e);
  }

  CONCEPT_REQUIRES(Resizable<Storage>{})
  void push_back_edge(edge_value_type const& e) {
    return push_back_edge(e, storage_type{});
  }

  template <typename Rng,
            CONCEPT_REQUIRES_(Range<Rng>{} and Resizable<Storage>{})>
  auto push_back_edges(Rng&& es) {
    reserve(edge_size() + ranges::size(es));
    for (auto&& e : es) { push_back_edge(e); }
  }

  /// Removes the last edge from the polyline
  ///
  /// TODO: should this return the last edge?
  CONCEPT_REQUIRES(Resizable<Storage>{})
  auto pop_back_edge() {
    HM3_ASSERT(edge_size() > 0, "cannot pop_back edge from empty polyline!");

    if constexpr (Same<point_based_t, storage_type>{}) {
      // If it contains only one edge, we pop out two points
      if (edge_size() == 1) { data_.pop_back(); }
    }
    data_.pop_back();
  }

  ///@}  // Edges

  CONCEPT_REQUIRES(Resizable<Storage>{})
  auto reserve(suint_t no_edges) {
    if constexpr (Same<point_based_t, storage_type>{}) {
      data_.reserve(no_edges + 1);
    } else if constexpr (Same<edge_based_t, storage_type>{}) {
      data_.reserve(no_edges);
    }
  }

  CONCEPT_REQUIRES(!Resizable<Storage>{})
  auto reserve(suint_t) {}

  /// \name Constructors/Assignment/Destructors
  ///@{

  polyline()                = default;
  polyline(polyline const&) = default;
  polyline(polyline&&)      = default;
  polyline& operator=(polyline const&) = default;
  polyline& operator=(polyline&&) = default;
  ~polyline()                     = default;

  /// Range-constructor from a range of edges (Resizable storage)
  template <
   typename Edges, typename ET = ranges::range_value_t<uncvref_t<Edges>>,
   CONCEPT_REQUIRES_(Range<Edges>{} and Constructible<edge_value_type, ET>{}
                     and Resizable<Storage>{})>
  explicit polyline(Edges&& es) noexcept {
    auto no_edges = ranges::distance(es);
    reserve(no_edges);
    for (auto&& e : es) { push_back_edge(e); }
  }

  /// Range-constructor from a range of edges (not Resizable storage)
  template <
   typename Edges, typename ET = ranges::range_value_t<uncvref_t<Edges>>,
   CONCEPT_REQUIRES_(Range<Edges>{} and Constructible<edge_value_type, ET>{}
                     and not Resizable<Storage>{})>
  explicit polyline(Edges&& es) noexcept {
    auto no_edges = ranges::distance(es);
    if constexpr (Same<point_based_t, storage_type>{}) {
      HM3_ASSERT(
       no_edges + 1 == ranges::size(data_),
       "no_edges: {}, no_points: {} (no_edges + 1), point_storage_size: {}",
       no_edges, no_edges + 1, ranges::size(data_));
      auto last
       = ranges::copy(es | view::transform([](auto&& e) { return e.x(0); }),
                      ranges::begin(data_));
      auto o = last.second;
      *(o)   = (*--ranges::end(es)).x(1);
    } else {
      static_assert(always_false<ET>{}, "not implemented");
    }
  }

  /// Assignment from range of edges (Resizable storage)
  template <
   typename Edges, typename ET = ranges::range_value_t<uncvref_t<Edges>>,
   CONCEPT_REQUIRES_(Range<Edges>{} and Constructible<edge_value_type, ET>{}
                     and Resizable<Storage>{})>
  polyline& operator=(Edges&& es) noexcept {
    data_.clear();
    reserve(ranges::distance(es));
    for (auto&& e : es) { push_back_edge(e); }
    return (*this);
  }

  ///@}  // Constructors/Assignment/Destructors

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};  // namespace polyline_primitive

template <dim_t Ad, typename S, typename D>
bool operator==(polyline<Ad, S, D> const& a,
                polyline<Ad, S, D> const& b) noexcept {
  if constexpr (HasData<polyline<Ad, S, D>>{}) {
    if (data(a) != data(b)) { return false; }
  }
  return equal(a.vertices(), b.vertices());
}

template <dim_t Ad, typename S, typename D>
bool operator!=(polyline<Ad, S, D> const& a,
                polyline<Ad, S, D> const& b) noexcept {
  return !(a == b);
}

/// \name VertexAccess
///@{

/// Number of vertices of the polyline \p p.
template <dim_t Ad, typename S, typename D>
constexpr auto vertex_size(polyline<Ad, S, D> const& p) noexcept {
  return p.vertex_size();
}

/// Vertex \p i of the polyline \p p.
template <dim_t Ad, typename S, typename D>
constexpr auto vertex(polyline<Ad, S, D> const& p, dim_t i) noexcept {
  HM3_ASSERT(i < p.vertex_size(), "");
  return p.vertex(i);
}

///@} // VertexAccess

/// \name EdgeAccess
///@{

/// Number of edges in the polyline \p p.
template <dim_t Ad, typename S, typename D>
constexpr auto edge_size(polyline<Ad, S, D> const& p) noexcept {
  return p.edge_size();
}

/// Edge \p e of the polyline \p p.
template <dim_t Ad, typename S, typename D>
constexpr auto edge(polyline<Ad, S, D> const& p, dim_t e) noexcept {
  HM3_ASSERT(e < p.edge_size(), "");
  return p.edge(e);
}

///@}  // EdgeAccess

}  // namespace polyline_primitive

}  // namespace hm3::geometry
