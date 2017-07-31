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
/// \tparam Storage Vector-like container for storing the polygon vertices or
/// edges.
template <dim_t Ad, typename Storage, typename Data>
struct polygon : private polyline_primitive::polyline<Ad, Storage, Data> {
  static_assert(Ad > 1, "Polygons do not exist in 1D ambient space!");
  using geometry_type = trait::polygon<Ad>;

  using polyline_t = polyline_primitive::polyline<Ad, Storage, Data>;

  using typename polyline_t::edge_value_type;
  using typename polyline_t::point_value_type;
  using typename polyline_t::storage_type;

  using polyline_t::edge;
  using polyline_t::edge_size;
  using polyline_t::edges;
  using polyline_t::get_primitive_data;
  using polyline_t::reserve;

  using face_value_type = polygon<Ad, Storage, Data>;

  /// \name Vertices
  ///@{

  /// Number of vertices in the polygon.
  auto vertex_size() const -> dim_t { return polyline_t::vertex_size() - 1; }

  /// Vertex \p i of the polygon.
  auto vertex(dim_t i) const {
    HM3_ASSERT(i < vertex_size(), "vertex index {} is out-of-bounds [0, {})", i,
               vertex_size());
    return polyline_t::vertex(i);
  }

  /// Vertices of the polygon
  ///
  /// TODO: should this drop the last vertex?
  auto vertices() const { return polyline_t::vertices(); }

  ///@} // Vertices

 private:
  template <typename At>
  constexpr void assert_invariants(At&& at) const noexcept {
    HM3_ASSERT_AT(
     edge_size() > 2,
     "Trying to construct a polygon with {} edges (requires at least 3 edges)",
     at, edge_size());
    HM3_ASSERT_AT(approx_point(polyline_t::vertex(vertex_size()), vertex(0)),
                  "first {} and last {} vertices of a polygon must coincide!",
                  at, polyline_t::vertex(vertex_size()), vertex(0));
    // if constexpr (Ad == 2) {
    //   HM3_ASSERT_AT(vertex_order(*this) == vertex_order_t::ccw,
    //                 "2D polygons must have its vertices ordered in ccw "
    //                 "order!\n\npoly: {}\n",
    //                 std::forward<At>(at),
    //                 static_cast<polyline_t const&>(*this));
    // }
  }

  // Due to round-off errors the last point of the polygon might not be bitwise
  // equal to the first one. When the points are approximately equal, this
  // function corrects this by forcing the last point to be bitwise equal to the
  // first one.
  void correct_last_point() {
    HM3_ASSERT(approx_point(polyline_t::vertex(vertex_size()), vertex(0)),
               "the polygon is not closed! The first and last vertices of the "
               "polygon are not approximately equal. First vertex: {}. Last "
               "Vertex: {}",
               vertex(0), polyline_t::vertex(vertex_size()));
    if (polyline_t::vertex(vertex_size()) != vertex(0)) {
      if constexpr (Same<point_based_t, storage_type>{}) {
        this->data_[vertex_size()] = vertex(0);
      } else if constexpr (Same<edge_based_t, storage_type>{}) {
        this->data_[vertex_size() - 1].x(1) = vertex(0);
      }
    }
  }

 public:
  /// \name Constructors/Assignment/Conversion/Destructor
  ///@{

  polygon()               = default;
  polygon(polygon const&) = default;
  polygon(polygon&&)      = default;
  polygon& operator=(polygon const&) = default;
  polygon& operator=(polygon&&) = default;
  ~polygon()                    = default;

  template <
   typename Edges, typename ET = ranges::range_value_t<uncvref_t<Edges>>,
   CONCEPT_REQUIRES_(Range<Edges>{} and Constructible<edge_value_type, ET>{})>
  explicit polygon(Edges&& es) noexcept : polyline_t(std::forward<Edges>(es)) {
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

  ///@} // Constructors/Assignment/Conversion/Destructor

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  /// Sets the c-th component of the vidx-th vertex of the polygon \p p to \p
  /// value.
  friend constexpr bool set_vertex(polygon<Ad, Storage>& p, dim_t vidx, dim_t c,
                                   num_t value) {
    if constexpr (Same<edge_based_t, storage_type>{}) {
      static_assert(always_false<Storage>{},
                    "not implemented for edge based types");
    }

    HM3_ASSERT(vidx < p.vertex_size(), "");
    HM3_ASSERT(c < Ad, "");
    p.data_[vidx](c) = value;
    if (vidx == 0) { p.data_[p.vertex_size()](c) = value; }
    return true;
  }
};

template <dim_t Ad, typename S, typename D>
bool operator==(polygon<Ad, S, D> const& a,
                polygon<Ad, S, D> const& b) noexcept {
  if constexpr (HasData<polygon<Ad, S, D>>{}) {
    if (data(a) != data(b)) { return false; }
  }
  return equal(a.vertices(), b.vertices());
}

template <dim_t Ad, typename S, typename D>
bool operator!=(polygon<Ad, S, D> const& a,
                polygon<Ad, S, D> const& b) noexcept {
  return !(a == b);
}

/// \name VertexAccess
///@{

/// Number of vertices of the polygon \p p.
template <dim_t Ad, typename S, typename D>
constexpr auto vertex_size(polygon<Ad, S, D> const& p) noexcept {
  return p.vertex_size();
}

/// Vertex \p i of the polygon \p p.
template <dim_t Ad, typename S, typename D>
constexpr auto vertex(polygon<Ad, S, D> const& p, dim_t i) noexcept {
  HM3_ASSERT(i < p.vertex_size(), "");
  return p.vertex(i);
}

///@} // VertexAccess

/// \name EdgeAccess
///@{

/// Number of edges in the polygon \p p.
template <dim_t Ad, typename S, typename D>
constexpr auto edge_size(polygon<Ad, S, D> const& p) noexcept {
  return p.edge_size();
}

/// Edge \p e of the polygon \p p.
template <dim_t Ad, typename S, typename D>
constexpr auto edge(polygon<Ad, S, D> const& p, dim_t e) noexcept {
  HM3_ASSERT(e < p.edge_size(), "");
  return p.edge(e);
}

///@}  // EdgeAccess

}  // namespace polygon_primitive

}  // namespace hm3::geometry
