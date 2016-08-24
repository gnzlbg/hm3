#pragma once
/// \file
///
/// A bounded polygon: contains a limited number of vertices.
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/polygon/ostream.hpp>
#include <hm3/geometry/rank.hpp>
#include <hm3/utility/small_vector.hpp>

namespace hm3::geometry {

namespace polygon_primitive {

/// Small polygon
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam EstNp Maximum expected number of vertices that the polygon
/// can contain.
template <dim_t Nd, dim_t EstNp>
struct small_polygon : ranked<Nd, 2>, private small_vector<point<Nd>, EstNp> {
  static_assert(EstNp > 2, "");
  using vertex_index_type = dim_t;
  using face_index_type   = dim_t;

  using vector_t = small_vector<point<Nd>, EstNp>;
  using vector_t::vector_t;
  using vector_t::operator=;
  using vector_t::size;
  using vector_t::empty;
  using vector_t::push_back;
  using vector_t::pop_back;
  using vector_t::operator[];
  using vector_t::insert;
  using vector_t::begin;
  using vector_t::end;
  using vector_t::reserve;
  using vector_t::max_size;

  small_polygon()                     = default;
  small_polygon(small_polygon const&) = default;
  small_polygon(small_polygon&&)      = default;
  small_polygon& operator=(small_polygon const&) = default;
  small_polygon& operator=(small_polygon&&) = default;

  template <typename Vertices,
            CONCEPT_REQUIRES_(!access::Vertex<Vertices>{} and Range<Vertices>{}
                              and !Polyline<Vertices>{})>
  small_polygon(Vertices&& vs) noexcept {
    HM3_ASSERT(ranges::distance(vs) > 2,
               "cannot construct polygon with less than 3 vertices: {}",
               view::all(vs));
    for (auto&& v : vs) { this->push_back(v); }
    HM3_ASSERT((*this)[0] != (*this)[size() - 1],
               "polygon's last vertex equals its first!");
  }

  template <typename Vertices,
            CONCEPT_REQUIRES_(!access::Vertex<Vertices>{} and Range<Vertices>{}
                              and !Polyline<Vertices>{})>
  small_polygon& operator=(Vertices&& vs) noexcept {
    HM3_ASSERT(ranges::distance(vs) > 2,
               "cannot construct polygon with less than 3 vertices: {}",
               view::all(vs));
    this->clear();
    for (auto&& v : std::forward<Vertices>(vs)) { this->push_back(v); }
    return (*this);
  }

  template <typename P, CONCEPT_REQUIRES_(
                         access::Vertex<P>{}  // and !Range<P>{}
                         and !Same<small_polygon<Nd, EstNp>, uncvref_t<P>>{}
                         and !Polyline<P>{})>
  small_polygon(P&& p) noexcept : small_polygon(geometry::vertices(p)) {}

  template <typename P, CONCEPT_REQUIRES_(
                         access::Vertex<P>{}  // and !Range<P>{}
                         and !Same<small_polygon<Nd, EstNp>, uncvref_t<P>>{}
                         and !Polyline<P>{})>
  small_polygon& operator=(P&& p) noexcept {
    (*this) = geometry::vertices(std::forward<P>(p));
    return (*this);
  }

  vector_t const& vertices() const & {
    return static_cast<vector_t const&>(*this);
  }
  vector_t& vertices() & { return static_cast<vector_t&>(*this); }
  vector_t vertices() && { return static_cast<vector_t&&>(*this); }
};

template <dim_t Nd, dim_t EstNp>
bool operator==(small_polygon<Nd, EstNp> const& a,
                small_polygon<Nd, EstNp> const& b) noexcept {
  return equal(a, b);
}

template <dim_t Nd, dim_t EstNp>
bool operator!=(small_polygon<Nd, EstNp> const& a,
                small_polygon<Nd, EstNp> const& b) noexcept {
  return !(a == b);
}

template <dim_t Nd, dim_t EstNp>
constexpr auto vertex_capacity(small_polygon<Nd, EstNp> const& p) noexcept {
  return p.capacity();
}

template <dim_t Nd, dim_t EstNp>
constexpr auto vertex_size(small_polygon<Nd, EstNp> const& p) noexcept {
  return p.size();
}

/// Polygon vertices
template <dim_t Nd, dim_t EstNp>
constexpr decltype(auto) vertices(small_polygon<Nd, EstNp> const& p) noexcept {
  return p.vertices();
}

template <dim_t Nd, dim_t EstNp>
constexpr decltype(auto) vertices(small_polygon<Nd, EstNp>& p) noexcept {
  return p.vertices();
}

template <dim_t Nd, dim_t EstNp>
constexpr decltype(auto) vertices(small_polygon<Nd, EstNp>&& p) noexcept {
  return p.vertices();
}

/// Polygon vertices
template <dim_t Nd, dim_t EstNp>
constexpr auto vertex(small_polygon<Nd, EstNp> p, dim_t v) noexcept {
  return p[v];
}

/// Vertex indices
template <dim_t Nd, dim_t EstNp>
auto vertex_indices(small_polygon<Nd, EstNp> const& p) noexcept {
  return view::iota(std::size_t{0}, p.size());
}

/// OStream operator
template <typename OStream, dim_t Nd, dim_t EstNp>
OStream& operator<<(OStream& os, small_polygon<Nd, EstNp> const& p) noexcept {
  return write_to_ostream(os, p);
}

}  // namespace polygon_primitive

using polygon_primitive::small_polygon;

}  // namespace hm3::geometry
