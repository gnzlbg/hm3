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

/// Polygon
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam Storage Vector-like storage of the polygon points.
template <dim_t Nd, typename Storage>
struct polygon : ranked<Nd, 2>, protected Storage {
  static_assert(Same<point<Nd>, typename Storage::value_type>{}, "");
  using vertex_index_type = dim_t;
  using face_index_type   = dim_t;

  using vector_t = Storage;
  using vector_t::vector_t;
  using vector_t::operator=;
  using vector_t::size;

  using vector_t::empty;
  using vector_t::operator[];
  using vector_t::begin;
  using vector_t::end;
  using vector_t::max_size;

  polygon()               = default;
  polygon(polygon const&) = default;
  polygon(polygon&&)      = default;
  polygon& operator=(polygon const&) = default;
  polygon& operator=(polygon&&) = default;

  CONCEPT_REQUIRES(Reservable<Storage>{})
  auto reserve(suint_t n) { return static_cast<Storage*>(this)->reserve(n); }

  CONCEPT_REQUIRES(!Reservable<Storage>{})
  auto reserve(suint_t) {}

  template <typename T, CONCEPT_REQUIRES_(PushBackable<Storage, T>{})>
  auto push_back(T&& t) {
    return static_cast<Storage*>(this)->push_back(std::forward<T>(t));
  }

  CONCEPT_REQUIRES(PopBackable<Storage>{})
  auto pop_back() { return static_cast<Storage*>(this)->pop_back(); }

  template <typename... Args,
            CONCEPT_REQUIRES_(PushBackable<Storage, point<Nd>>{})>
  auto insert(Args&&... args) {
    static_cast<Storage*>(this)->insert(std::forward<Args>(args)...);
  }

  /// Insert vertices if the polygon has push_back
  template <typename Vertices,
            CONCEPT_REQUIRES_(!access::Vertex<Vertices>{} and Range<Vertices>{}
                              and !Polyline<Vertices>{}
                              and PushBackable<Storage, point<Nd>>{})>
  polygon(Vertices&& vs) noexcept {
    auto no_vertices = ranges::distance(vs);
    HM3_ASSERT(no_vertices > 2,
               "cannot construct polygon with less than 3 vertices: {}",
               view::all(vs));
    HM3_ASSERT(
     static_cast<suint_t>(no_vertices) <= max_size(),
     "number of vertices {} exceeds the maximum size of the polygon {}",
     no_vertices, max_size());
    reserve(no_vertices);
    for (auto&& v : vs) { this->push_back(v); }
    HM3_ASSERT((*this)[0] != (*this)[size() - 1],
               "polygon's last vertex equals its first!");
  }

  /// Insert vertices if the polygon does not have push_back
  template <typename Vertices,
            CONCEPT_REQUIRES_(!access::Vertex<Vertices>{} and Range<Vertices>{}
                              and !Polyline<Vertices>{}
                              and !PushBackable<Storage, point<Nd>>{})>
  polygon(Vertices&& vs) noexcept {
    HM3_ASSERT(ranges::distance(vs) == this->size(),
               "trying to construct a fixed-size polygon of size {} with a "
               "range of size {}",
               this->size(), ranges::distance(vs));
    ranges::copy(vs, ranges::begin(*this));
    HM3_ASSERT((*this)[0] != (*this)[size() - 1],
               "polygon's last vertex equals its first!");
  }

  template <typename Vertices,
            CONCEPT_REQUIRES_(!access::Vertex<Vertices>{} and Range<Vertices>{}
                              and !Polyline<Vertices>{}
                              and PushBackable<Storage, point<Nd>>{})>
  polygon& operator=(Vertices&& vs) noexcept {
    auto no_vertices = ranges::distance(vs);
    HM3_ASSERT(no_vertices > 2,
               "cannot construct polygon with less than 3 vertices: {}",
               view::all(vs));
    HM3_ASSERT(
     static_cast<suint_t>(no_vertices) <= max_size(),
     "number of vertices {} exceeds the maximum size of the polygon {}",
     no_vertices, max_size());
    this->clear();
    reserve(no_vertices);
    for (auto&& v : std::forward<Vertices>(vs)) { this->push_back(v); }
    HM3_ASSERT((*this)[0] != (*this)[size() - 1],
               "polygon's last vertex equals its first!");
    return (*this);
  }

  template <typename Vertices,
            CONCEPT_REQUIRES_(!access::Vertex<Vertices>{} and Range<Vertices>{}
                              and !Polyline<Vertices>{}
                              and !PushBackable<Storage, point<Nd>>{})>
  polygon& operator=(Vertices&& vs) noexcept {
    HM3_ASSERT(ranges::distance(vs) == this->size(),
               "trying to construct a fixed-size polygon of size {} with a "
               "range of size {}",
               this->size(), ranges::distance(vs));
    ranges::copy(vs, ranges::begin(*this));
    HM3_ASSERT((*this)[0] != (*this)[size() - 1],
               "polygon's last vertex equals its first!");
    return (*this);
  }

  template <typename P,
            CONCEPT_REQUIRES_(access::Vertex<P>{}  // and !Range<P>{}
                              and !Same<polygon<Nd, Storage>, uncvref_t<P>>{}
                              and !Polyline<P>{})>
  polygon(P&& p) noexcept : polygon(geometry::vertices(p)) {}

  template <typename P,
            CONCEPT_REQUIRES_(access::Vertex<P>{}  // and !Range<P>{}
                              and !Same<polygon<Nd, Storage>, uncvref_t<P>>{}
                              and !Polyline<P>{})>
  polygon& operator=(P&& p) noexcept {
    (*this) = geometry::vertices(std::forward<P>(p));
    return (*this);
  }

  vector_t const& vertices() const & {
    return static_cast<vector_t const&>(*this);
  }
  vector_t& vertices() & { return static_cast<vector_t&>(*this); }
  vector_t vertices() && { return static_cast<vector_t&&>(*this); }
};

template <dim_t Nd, typename Storage>
bool operator==(polygon<Nd, Storage> const& a,
                polygon<Nd, Storage> const& b) noexcept {
  return equal(a, b);
}

template <dim_t Nd, typename Storage>
bool operator!=(polygon<Nd, Storage> const& a,
                polygon<Nd, Storage> const& b) noexcept {
  return !(a == b);
}

/// Number of polygon vertices.
template <dim_t Nd, typename Storage>
constexpr auto vertex_size(polygon<Nd, Storage> const& p) noexcept {
  return p.size();
}

/// Polygon vertices.
template <dim_t Nd, typename Storage>
constexpr decltype(auto) vertices(polygon<Nd, Storage> const& p) noexcept {
  return p.vertices();
}

/// Polygon vertices.
template <dim_t Nd, typename Storage>
constexpr decltype(auto) vertices(polygon<Nd, Storage>& p) noexcept {
  return p.vertices();
}

/// Polygon vertices.
template <dim_t Nd, typename Storage>
constexpr decltype(auto) vertices(polygon<Nd, Storage>&& p) noexcept {
  return p.vertices();
}

/// Vertex \p v of the polygon \p p.
template <dim_t Nd, typename Storage>
constexpr decltype(auto) vertex(polygon<Nd, Storage> const& p,
                                dim_t v) noexcept {
  return p[v];
}

/// Vertex \p v of the polygon \p p.
template <dim_t Nd, typename Storage>
constexpr decltype(auto) vertex(polygon<Nd, Storage>& p, dim_t v) noexcept {
  return p[v];
}

/// Vertex \p v of the polygon \p p.
template <dim_t Nd, typename Storage>
constexpr decltype(auto) vertex(polygon<Nd, Storage>&& p, dim_t v) noexcept {
  return p[v];
}

/// Vertex indices.
template <dim_t Nd, typename Storage>
auto vertex_indices(polygon<Nd, Storage> const& p) noexcept {
  return view::iota(std::size_t{0}, p.size());
}

/// OStream operator
template <typename OStream, dim_t Nd, typename Storage>
OStream& operator<<(OStream& os, polygon<Nd, Storage> const& p) noexcept {
  return write_to_ostream(os, p);
}

}  // namespace polygon_primitive

using polygon_primitive::polygon;

}  // namespace hm3::geometry
