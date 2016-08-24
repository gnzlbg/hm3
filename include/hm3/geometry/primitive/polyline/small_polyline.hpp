#pragma once
/// \file
///
/// Polyline.
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/polyline/ostream.hpp>
#include <hm3/geometry/rank.hpp>
#include <hm3/utility/small_vector.hpp>

namespace hm3::geometry {

namespace polyline_primitive {

/// Small polyline
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam EstNp Maximum expected number of vertices that the polyline
/// can contain.
template <dim_t Nd, dim_t EstNp>
struct small_polyline : ranked<Nd, 1>, private small_vector<point<Nd>, EstNp> {
  static_assert(EstNp > 1, "");
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

  small_polyline()                      = default;
  small_polyline(small_polyline const&) = default;
  small_polyline(small_polyline&&)      = default;
  small_polyline& operator=(small_polyline const&) = default;
  small_polyline& operator=(small_polyline&&) = default;

  template <typename Vertices, CONCEPT_REQUIRES_(!access::Vertex<Vertices>{}
                                                 and Range<Vertices>{})>
  small_polyline(Vertices&& vs) noexcept {
    HM3_ASSERT(ranges::distance(vs) > 1,
               "cannot construct polyline with less than 2 vertices: {}",
               view::all(vs));
    HM3_ASSERT(
     (dim_t)ranges::distance(vs) <= EstNp,
     "polyline vertex capacity is {} but the number of vertices is {}: {}",
     EstNp, ranges::distance(vs), view::all(vs));
    for (auto&& v : vs) { this->push_back(v); }
  }

  template <typename Vertices, CONCEPT_REQUIRES_(!access::Vertex<Vertices>{}
                                                 and Range<Vertices>{})>
  small_polyline& operator=(Vertices&& vs) noexcept {
    HM3_ASSERT(ranges::distance(vs) > 1,
               "cannot construct polyline with less than 2 vertices: {}",
               view::all(vs));
    HM3_ASSERT(
     (dim_t)ranges::distance(vs) <= EstNp,
     "polyline vertex capacity is {} but the number of vertices is {}: {}",
     EstNp, ranges::distance(vs), view::all(vs));
    this->clear();
    for (auto&& v : std::forward<Vertices>(vs)) { this->push_back(v); }
    return (*this);
  }

  template <typename P, CONCEPT_REQUIRES_(
                         access::Vertex<P>{}  // and !Range<P>{}
                         and !Same<small_polyline<Nd, EstNp>, uncvref_t<P>>{})>
  small_polyline(P&& p) noexcept : small_polyline(geometry::vertices(p)) {}

  template <typename P, CONCEPT_REQUIRES_(
                         access::Vertex<P>{}  // and !Range<P>{}
                         and !Same<small_polyline<Nd, EstNp>, uncvref_t<P>>{})>
  small_polyline& operator=(P&& p) noexcept {
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
bool operator==(small_polyline<Nd, EstNp> const& a,
                small_polyline<Nd, EstNp> const& b) noexcept {
  return equal(a, b);
}

template <dim_t Nd, dim_t EstNp>
bool operator!=(small_polyline<Nd, EstNp> const& a,
                small_polyline<Nd, EstNp> const& b) noexcept {
  return !(a == b);
}

template <dim_t Nd, dim_t EstNp>
constexpr auto vertex_capacity(small_polyline<Nd, EstNp> const& p) noexcept {
  return p.capacity();
}

template <dim_t Nd, dim_t EstNp>
constexpr auto vertex_size(small_polyline<Nd, EstNp> const& p) noexcept {
  return p.size();
}

/// Polyline vertices
template <dim_t Nd, dim_t EstNp>
constexpr decltype(auto) vertices(small_polyline<Nd, EstNp> const& p) noexcept {
  return p.vertices();
}

template <dim_t Nd, dim_t EstNp>
constexpr decltype(auto) vertices(small_polyline<Nd, EstNp>& p) noexcept {
  return p.vertices();
}

template <dim_t Nd, dim_t EstNp>
constexpr decltype(auto) vertices(small_polyline<Nd, EstNp>&& p) noexcept {
  return p.vertices();
}

/// Polyline vertices
template <dim_t Nd, dim_t EstNp>
constexpr auto vertex(small_polyline<Nd, EstNp> p, dim_t v) noexcept {
  return p[v];
}

/// Vertex indices
template <dim_t Nd, dim_t EstNp>
auto vertex_indices(small_polyline<Nd, EstNp> const& p) noexcept {
  return view::iota(std::size_t{0}, p.size());
}

/// OStream operator
template <typename OStream, dim_t Nd, dim_t EstNp>
OStream& operator<<(OStream& os, small_polyline<Nd, EstNp> const& p) noexcept {
  return write_to_ostream(os, p);
}

}  // namespace polyline_primitive

using polyline_primitive::small_polyline;

}  // namespace hm3::geometry
