#pragma once
/// \file
///
/// Polyline.
#include <hm3/geometry/concept/resizable.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/geometry/primitive/detail/vertex_storage.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/io/ascii.hpp>
#include <hm3/utility/small_vector.hpp>

namespace hm3::geometry {

/// Polyline primitive
namespace polyline_primitive {

/// Polyline.
///
/// \tparam Ad Ambient space dimension.
/// \tparam PointStorage Vector-like container for storing the polyline
/// vertices.
template <dim_t Ad, typename PointStorage>
struct polyline {
  static_assert(Same<typename PointStorage::value_type, point<Ad>>{},
                "error: polyline assumes the value type of the storage "
                "to be of type point<Ad>");
  // Storage for the vertices
  using data_t          = vertex_storage_detail::vertex_storage<PointStorage>;
  using geometry_type   = trait::polyline<Ad>;
  using edge_value_type = segment<Ad>;
  using concatenated_value_type = polyline<Ad>;
  data_t data_;

  polyline()                = default;
  polyline(polyline const&) = default;
  polyline(polyline&&)      = default;
  polyline& operator=(polyline const&) = default;
  polyline& operator=(polyline&&) = default;
  ~polyline()                     = default;

  template <
   typename Edges, typename ET = ranges::range_value_t<uncvref_t<Edges>>,
   CONCEPT_REQUIRES_(Range<Edges>{} and Constructible<edge_value_type, ET>{}
                     and Resizable<PointStorage>{})>
  explicit polyline(Edges&& es) noexcept {
    auto no_edges = ranges::distance(es);
    reserve(no_edges);
    for (auto&& e : es) { push_back_edge(e); }
  }

  template <
   typename Edges, typename ET = ranges::range_value_t<uncvref_t<Edges>>,
   CONCEPT_REQUIRES_(Range<Edges>{} and Constructible<edge_value_type, ET>{}
                     and not Resizable<PointStorage>{})>
  explicit polyline(Edges&& es) noexcept {
    auto no_edges = ranges::distance(es);
    HM3_ASSERT(
     no_edges + 1 == ranges::size(data_),
     "no_edges: {}, no_points: {} (no_edges + 1), point_storage_size: {}",
     no_edges, no_edges + 1, ranges::size(data_));
    auto last
     = ranges::copy(es | view::transform([](auto&& e) { return e.x(0); }),
                    ranges::begin(data_));
    auto o = last.second;
    *(o)   = (*--ranges::end(es)).x(1);
  }

  template <
   typename Edges, typename ET = ranges::range_value_t<uncvref_t<Edges>>,
   CONCEPT_REQUIRES_(Range<Edges>{} and Constructible<edge_value_type, ET>{}
                     and Resizable<PointStorage>{})>
  polyline& operator=(Edges&& es) noexcept {
    auto no_edges = ranges::distance(es);
    data_.clear();
    data_.reserve(no_edges);
    for (auto&& e : std::forward<Edges>(es)) { push_back_edge(e); }
    return (*this);
  }

  auto vertex_size() const -> dim_t { return data_.size(); }
  auto edge_size() const -> dim_t {
    if (vertex_size() < 2) { return 0; }
    return vertex_size() - 1;
  }

  auto vertex(dim_t i) const {
    HM3_ASSERT(i < vertex_size(), "vertex index {} is out-of-bounds [0, {})", i,
               vertex_size());
    return data_[i];
  }
  auto edge(dim_t i) const {
    HM3_ASSERT(i < edge_size(), "edge index {} is out-of-bounds [0, {})", i,
               edge_size());
    edge_value_type e(vertex(i), vertex(i + 1));
    return e;
  }

  auto vertices() const {
    return view::iota(0, vertex_size())
           | view::transform([&](auto&& i) { return vertex(i); });
  }
  auto edges() const {
    return view::iota(0, edge_size())
           | view::transform([&](auto&& i) { return edge(i); });
  }

  CONCEPT_REQUIRES(Resizable<PointStorage>{})
  auto reserve(suint_t no_edges) { data_.reserve(no_edges + 1); }

  CONCEPT_REQUIRES(!Resizable<PointStorage>{})
  auto reserve(suint_t) {}

  CONCEPT_REQUIRES(Resizable<PointStorage>{})
  void push_back_edge(edge_value_type const& e) {
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

  template <typename Rng,
            CONCEPT_REQUIRES_(Range<Rng>{} and Resizable<PointStorage>{})>
  auto push_back_edges(Rng&& es) {
    reserve(edge_size() + ranges::size(es));
    for (auto&& e : es) { push_back_edge(e); }
  }

  CONCEPT_REQUIRES(Resizable<PointStorage>{})
  auto pop_back_edge() {
    HM3_ASSERT(edge_size() > 0, "cannot pop_back edge from empty polyline!");
    auto e = edge_value_type(edge(data_.size() - 1), edge(data_.size() - 2));

    if (edge_size() == 1) { data_.pop_back(); }
    data_.pop_back();
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <dim_t Ad, typename S>
bool operator==(polyline<Ad, S> const& a, polyline<Ad, S> const& b) noexcept {
  return equal(a.vertices(), b.vertices());
}

template <dim_t Ad, typename S>
bool operator!=(polyline<Ad, S> const& a, polyline<Ad, S> const& b) noexcept {
  return !(a == b);
}

/// \name VertexAccess
///@{

/// Number of vertices of the polyline \p p.
template <dim_t Ad, typename S>
constexpr auto vertex_size(polyline<Ad, S> const& p) noexcept {
  return p.vertex_size();
}

/// Vertex \p i of the polyline \p p.
template <dim_t Ad, typename S>
constexpr auto vertex(polyline<Ad, S> const& p, dim_t i) noexcept {
  HM3_ASSERT(i < p.vertex_size(), "");
  return p.vertex(i);
}

///@} // VertexAccess

/// \name EdgeAccess
///@{

/// Number of edges in the polyline \p p.
template <dim_t Ad, typename S>
constexpr auto edge_size(polyline<Ad, S> const& p) noexcept {
  return p.edge_size();
}

/// Edge \p e of the polyline \p p.
template <dim_t Ad, typename S>
constexpr auto edge(polyline<Ad, S> const& p, dim_t e) noexcept {
  HM3_ASSERT(e < p.edge_size(), "");
  return p.edge(e);
}

///@}  // EdgeAccess

}  // namespace polyline_primitive

using polyline_primitive::polyline;

}  // namespace hm3::geometry
