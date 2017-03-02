#pragma once
/// \file
///
/// A closed polygon.
#include <hm3/geometry/algorithm/vertex_order.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/io/ascii.hpp>
#include <hm3/utility/small_vector.hpp>

namespace hm3::geometry {

namespace polygon_primitive {

/// Closed polygon.
///
/// In 2D the vertices are ordered ccw.
///
/// \tparam Ad Number of spatial dimensions.
/// \tparam PointStorage Vector-like container for storing the polygon vertices.
template <dim_t Ad, typename PointStorage>
struct polygon : private polyline<Ad, PointStorage> {
  static_assert(Ad > 1, "Polygons do not exist in 1D ambient space!");
  using geometry_type = trait::polygon<Ad>;

  using polyline_t = polyline<Ad, PointStorage>;
  using typename polyline_t::edge_value_type;

  polygon()               = default;
  polygon(polygon const&) = default;
  polygon(polygon&&)      = default;
  polygon& operator=(polygon const&) = default;
  polygon& operator=(polygon&&) = default;

  using polyline_t::edge_size;
  using polyline_t::edge;
  using polyline_t::vertices;
  using polyline_t::edges;
  using polyline_t::reserve;

  auto vertex_size() const -> dim_t { return this->data_.size() - 1; }
  auto vertex(dim_t i) const {
    HM3_ASSERT(i < vertex_size(), "vertex index {} is out-of-bounds [0, {})", i,
               vertex_size());
    return this->data_[i];
  }

  template <typename At>
  constexpr void assert_invariants(At&& at) const noexcept {
    HM3_ASSERT_AT(
     edge_size() > 2,
     "Trying to construct a polygon with {} edges (requires at least 3 edges)",
     at, edge_size());
    HM3_ASSERT_AT(approx_point(this->data_[vertex_size()], vertex(0)),
                  "first {} and last {} vertices of a polygon must coincide!",
                  at, this->data_[vertex_size()], vertex(0));
    /*if
      constexpr(Ad == 2) {
        HM3_ASSERT_AT(vertex_order(*this) == vertex_order_t::ccw,
                      "2D polygons must have its vertices ordered in ccw "
                      "order!\n\npoly: {}\n",
                      std::forward<At>(at),
                      static_cast<polyline_t const&>(*this));
                      }*/
  }

  // Due to round-off errors the last point of the polygon might not be bitwise
  // equal to the first one. When the points are approximately equal, this
  // function corrects this by forcing the last point to be bitwise equal to the
  // first one.
  void correct_last_point() {
    HM3_ASSERT(approx_point(this->data_[vertex_size()], vertex(0)), "");
    if (this->data_[vertex_size()] != vertex(0)) {
      this->data_[vertex_size()] = vertex(0);
    }
  }

  template <
   typename Edges, typename ET = ranges::range_value_t<uncvref_t<Edges>>,
   CONCEPT_REQUIRES_(Range<Edges>{} and Constructible<edge_value_type, ET>{})>
  polygon(Edges&& es) noexcept : polyline_t(std::forward<Edges>(es)) {
    assert_invariants(HM3_AT_);
    correct_last_point();
  }

  template <
   typename Edges, typename ET = ranges::range_value_t<uncvref_t<Edges>>,
   CONCEPT_REQUIRES_(Range<Edges>{} and Constructible<edge_value_type, ET>{})>
  polygon& operator=(Edges&& es) noexcept {
    static_cast<polyline_t&>(*this) = std::forward<Edges>(es);
    assert_invariants(HM3_AT_);
    correct_last_point();
    return (*this);
  }

  /// Construct from a closed polyline.
  explicit polygon(polyline_t pl) : polyline_t(std::move(pl)) {
    assert_invariants(HM3_AT_);
    correct_last_point();
  }

  /// Convert to a polyline.
  polyline_t polyline() const { return static_cast<polyline_t const&>(*this); }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  friend constexpr bool set_vertex(polygon<Ad, PointStorage>& p, dim_t vidx,
                                   dim_t c, num_t value) {
    HM3_ASSERT(vidx < p.vertex_size(), "");
    HM3_ASSERT(c < Ad, "");
    p.data_[vidx](c) = value;
    if (vidx == 0) { p.data_[p.vertex_size()](c) = value; }
    return true;
  }
};

template <dim_t Ad, typename S>
bool operator==(polygon<Ad, S> const& a, polygon<Ad, S> const& b) noexcept {
  return equal(a.vertices(), b.vertices());
}

template <dim_t Ad, typename S>
bool operator!=(polygon<Ad, S> const& a, polygon<Ad, S> const& b) noexcept {
  return !(a == b);
}

/// \name VertexAccess
///@{

/// Number of vertices of the polygon \p p.
template <dim_t Ad, typename S>
constexpr auto vertex_size(polygon<Ad, S> const& p) noexcept {
  return p.vertex_size();
}

/// Vertex \p i of the polygon \p p.
template <dim_t Ad, typename S>
constexpr auto vertex(polygon<Ad, S> const& p, dim_t i) noexcept {
  HM3_ASSERT(i < p.vertex_size(), "");
  return p.vertex(i);
}

///@} // VertexAccess

/// \name EdgeAccess
///@{

/// Number of edges in the polygon \p p.
template <dim_t Ad, typename S>
constexpr auto edge_size(polygon<Ad, S> const& p) noexcept {
  return p.edge_size();
}

/// Edge \p e of the polygon \p p.
template <dim_t Ad, typename S>
constexpr auto edge(polygon<Ad, S> const& p, dim_t e) noexcept {
  HM3_ASSERT(e < p.edge_size(), "");
  return p.edge(e);
}

///@}  // EdgeAccess

}  // namespace polygon_primitive

using polygon_primitive::polygon;

}  // namespace hm3::geometry
