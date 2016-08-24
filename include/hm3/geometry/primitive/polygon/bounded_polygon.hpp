#pragma once
/// \file
///
/// A bounded polygon: contains a limited number of vertices.
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/polygon/ostream.hpp>
#include <hm3/geometry/rank.hpp>
#include <hm3/utility/inline_vector.hpp>

namespace hm3::geometry {

namespace polygon_primitive {

/// Bounded polygon
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam MaxNv Maximum number of vertices that the polygon can contain.
template <dim_t Nd, dim_t MaxNv>
struct bounded_polygon : ranked<Nd, 2>,
                         private inline_vector<point<Nd>, MaxNv> {
  static_assert(MaxNv > 2, "");
  using vertex_index_type = dim_t;
  using face_index_type   = dim_t;

  using vector_t = inline_vector<point<Nd>, MaxNv>;
  using vector_t::vector_t;
  using vector_t::operator=;
  using vector_t::size;
  using vector_t::empty;
  using vector_t::push_back;
  using vector_t::pop_back;
  using vector_t::operator[];
  using vector_t::begin;
  using vector_t::end;
  using vector_t::max_size;

  bounded_polygon() = default;

  template <typename Vertices,
            CONCEPT_REQUIRES_(!access::Vertex<Vertices>{} and Range<Vertices>{}
                              and !Polyline<Vertices>{})>
  bounded_polygon(Vertices&& vs) noexcept {
    HM3_ASSERT(ranges::distance(vs) > 2,
               "cannot construct polygon with less than 3 vertices: {}",
               view::all(vs));
    HM3_ASSERT(
     (dim_t)ranges::distance(vs) <= MaxNv,
     "polygon vertex capacity is {} but the number of vertices is {}: {}",
     MaxNv, ranges::distance(vs), view::all(vs));
    for (auto&& v : vs) { this->push_back(v); }
    HM3_ASSERT((*this)[0] != (*this)[size() - 1],
               "polygon's last vertex equals its first!");
  }

  template <typename P, CONCEPT_REQUIRES_(
                         access::Vertex<P>{}  // and !Range<P>{}
                         and !Same<bounded_polygon<Nd, MaxNv>, uncvref_t<P>>{}
                         and !Polyline<P, Nd>{})>
  bounded_polygon(P&& p) noexcept : bounded_polygon(geometry::vertices(p)) {}

  vector_t const& vertices() const & noexcept {
    return static_cast<vector_t const&>(*this);
  }
  vector_t& vertices() & noexcept { return static_cast<vector_t&>(*this); }
  vector_t vertices() && noexcept { return static_cast<vector_t&&>(*this); }
};

template <dim_t Nd, dim_t MaxNv>
bool operator==(bounded_polygon<Nd, MaxNv> const& a,
                bounded_polygon<Nd, MaxNv> const& b) noexcept {
  return equal(a, b);
}

template <dim_t Nd, dim_t MaxNv>
bool operator!=(bounded_polygon<Nd, MaxNv> const& a,
                bounded_polygon<Nd, MaxNv> const& b) noexcept {
  return !(a == b);
}

template <dim_t Nd, dim_t MaxNv>
constexpr auto vertex_capacity(bounded_polygon<Nd, MaxNv> const&) noexcept {
  return MaxNv;
}

template <dim_t Nd, dim_t MaxNv>
constexpr auto vertex_size(bounded_polygon<Nd, MaxNv> const& p) noexcept {
  return p.size();
}

/// Polygon vertices
template <dim_t Nd, dim_t MaxNv>
constexpr decltype(auto) vertices(
 bounded_polygon<Nd, MaxNv> const& p) noexcept {
  return p.vertices();
}

template <dim_t Nd, dim_t MaxNv>
constexpr decltype(auto) vertices(bounded_polygon<Nd, MaxNv>&& p) noexcept {
  return p.vertices();
}

template <dim_t Nd, dim_t MaxNv>
constexpr decltype(auto) vertices(bounded_polygon<Nd, MaxNv>& p) noexcept {
  return p.vertices();
}

/// Polygon vertices
template <dim_t Nd, dim_t MaxNv>
constexpr auto vertex(bounded_polygon<Nd, MaxNv> p, dim_t v) noexcept {
  return p[v];
}

/// Vertex indices
template <dim_t Nd, dim_t MaxNv>
auto vertex_indices(bounded_polygon<Nd, MaxNv> const& p) noexcept {
  return view::iota(std::size_t{0}, p.size());
}

/// OStream operator
template <typename OStream, dim_t Nd, dim_t MaxNv>
OStream& operator<<(OStream& os, bounded_polygon<Nd, MaxNv> const& p) noexcept {
  return write_to_ostream(os, p);
}

}  // namespace polygon_primitive

using polygon_primitive::bounded_polygon;

}  // namespace hm3::geometry
