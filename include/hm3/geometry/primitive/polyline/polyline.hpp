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

/// Polyline
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam Storage Vector-like storage of the polygon points.
template <dim_t Nd, typename Storage>
struct polyline : ranked<Nd, 1>, protected Storage {
  static_assert(Same<point<Nd>, typename Storage::value_type>{}, "");
  using vertex_index_type = dim_t;
  using face_index_type   = dim_t;

  using vector_t = Storage;
  using vector_t::vector_t;
  using vector_t::operator=;
  using vector_t::size;

  using vector_t::empty;
  using vector_t::max_size;

 private:
  using vector_t::operator[];
  using vector_t::begin;
  using vector_t::end;

 public:
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

  polyline()                = default;
  polyline(polyline const&) = default;
  polyline(polyline&&)      = default;
  polyline& operator=(polyline const&) = default;
  polyline& operator=(polyline&&) = default;

  template <typename Vertices,
            CONCEPT_REQUIRES_(!access::Vertex<Vertices>{} and Range<Vertices>{}
                              and !Polygon<Vertices>{}
                              and PushBackable<Storage, point<Nd>>{})>
  polyline(Vertices&& vs) noexcept {
    auto no_vertices = ranges::distance(vs);
    HM3_ASSERT(no_vertices > 1,
               "cannot construct polyline with less than 2 vertices: {}",
               view::all(vs));
    HM3_ASSERT((dim_t)no_vertices <= max_size(),
               "number of vertices {} exceeds the maximum size  {}",
               no_vertices, max_size());
    reserve(no_vertices);
    for (auto&& v : vs) { this->push_back(v); }
  }

  template <typename Vertices,
            CONCEPT_REQUIRES_(!access::Vertex<Vertices>{} and Range<Vertices>{}
                              and !Polygon<Vertices>{}
                              and PushBackable<Storage, point<Nd>>{})>
  polyline& operator=(Vertices&& vs) noexcept {
    auto no_vertices = ranges::distance(vs);
    HM3_ASSERT(no_vertices > 1,
               "cannot construct polyline with less than 2 vertices: {}",
               view::all(vs));
    HM3_ASSERT((dim_t)no_vertices <= max_size(),
               "number of vertices {} exceeds the maximum size  {}",
               no_vertices, max_size());
    this->clear();
    reserve(no_vertices);
    for (auto&& v : std::forward<Vertices>(vs)) { this->push_back(v); }
    return (*this);
  }

  template <typename Vertices,
            CONCEPT_REQUIRES_(!access::Vertex<Vertices>{} and Range<Vertices>{}
                              and !Polygon<Vertices>{}
                              and !PushBackable<Storage, point<Nd>>{})>
  polyline(Vertices&& vs) noexcept {
    HM3_ASSERT(ranges::distance(vs) == this->size(),
               "trying to construct a fixed-size polygon of size {} with a "
               "range of size {}",
               this->size(), ranges::distance(vs));
    ranges::copy(vs, this->begin());
  }

  template <typename Vertices,
            CONCEPT_REQUIRES_(!access::Vertex<Vertices>{} and Range<Vertices>{}
                              and !Polygon<Vertices>{}
                              and !PushBackable<Storage, point<Nd>>{})>
  polyline& operator=(Vertices&& vs) noexcept {
    HM3_ASSERT(ranges::distance(vs) == this->size(),
               "trying to construct a fixed-size polygon of size {} with a "
               "range of size {}",
               this->size(), ranges::distance(vs));
    ranges::copy(vs, this->begin());
    return (*this);
  }

  template <typename P,
            CONCEPT_REQUIRES_(access::Vertex<P>{}  // and !Range<P>{}
                              and !Same<polyline<Nd, Storage>, uncvref_t<P>>{})>
  polyline(P&& p) noexcept : polyline(geometry::vertices(p)) {}

  template <typename P,
            CONCEPT_REQUIRES_(access::Vertex<P>{}  // and !Range<P>{}
                              and !Same<polyline<Nd, Storage>, uncvref_t<P>>{})>
  polyline& operator=(P&& p) noexcept {
    (*this) = geometry::vertices(std::forward<P>(p));
    return (*this);
  }

  vector_t const& vertices() const & {
    return static_cast<vector_t const&>(*this);
  }
  vector_t& vertices() & { return static_cast<vector_t&>(*this); }
  vector_t vertices() && { return static_cast<vector_t&&>(*this); }

  /// Polyline vertices.
  friend constexpr decltype(auto) vertex(polyline<Nd, Storage> const& p,
                                         dim_t v) noexcept {
    return p[v];
  }

  /// Polyline vertices.
  friend constexpr decltype(auto) vertex(polyline<Nd, Storage>& p,
                                         dim_t v) noexcept {
    return p[v];
  }

  /// Polyline vertices.
  friend constexpr decltype(auto) vertex(polyline<Nd, Storage>&& p,
                                         dim_t v) noexcept {
    return p[v];
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <dim_t Nd, typename S0, typename S1>
bool operator==(polyline<Nd, S0> const& a, polyline<Nd, S1> const& b) noexcept {
  return equal(a.vertices(), b.vertices());
}

template <dim_t Nd, typename S0, typename S1>
bool operator!=(polyline<Nd, S0> const& a, polyline<Nd, S1> const& b) noexcept {
  return !(a == b);
}

template <dim_t Nd, typename Storage>
constexpr auto vertex_size(polyline<Nd, Storage> const& p) noexcept {
  return p.size();
}

/// Polyline vertices.
template <dim_t Nd, typename Storage>
constexpr decltype(auto) vertices(polyline<Nd, Storage> const& p) noexcept {
  return p.vertices();
}

/// Polyline vertices.
template <dim_t Nd, typename Storage>
constexpr decltype(auto) vertices(polyline<Nd, Storage>& p) noexcept {
  return p.vertices();
}

/// Polyline vertices.
template <dim_t Nd, typename Storage>
constexpr decltype(auto) vertices(polyline<Nd, Storage>&& p) noexcept {
  return p.vertices();
}

/// Vertex indices
template <dim_t Nd, typename Storage>
auto vertex_indices(polyline<Nd, Storage> const& p) noexcept {
  return view::iota(std::size_t{0}, p.size());
}

/// OStream operator
template <typename OStream, dim_t Nd, typename Storage>
OStream& operator<<(OStream& os, polyline<Nd, Storage> const& p) noexcept {
  return write_to_ostream(os, p);
}

}  // namespace polyline_primitive

using polyline_primitive::polyline;

}  // namespace hm3::geometry
