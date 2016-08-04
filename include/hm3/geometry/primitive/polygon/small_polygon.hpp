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
/// \tparam MaxExpectedNv Maximum expected number of vertices that the polygon
/// can contain.
template <dim_t Nd, dim_t MaxExpectedNv>
struct small_polygon : ranked<Nd, 2>, small_vector<point<Nd>, MaxExpectedNv> {
  static_assert(MaxExpectedNv > 2, "");
  using vertex_index_type = dim_t;
  using face_index_type   = dim_t;

  using vector_t = small_vector<point<Nd>, MaxExpectedNv>;
  using vector_t::vector_t;
  using vector_t::operator=;

  small_polygon() = default;

  template <typename Vertices, CONCEPT_REQUIRES_(Range<Vertices>{})>
  small_polygon(Vertices&& vs) noexcept {
    HM3_ASSERT(ranges::distance(vs) > 2,
               "cannot construct polygon with less than 3 vertices: {}",
               view::all(vs));
    HM3_ASSERT(
     (dim_t)ranges::distance(vs) <= MaxExpectedNv,
     "polygon vertex capacity is {} but the number of vertices is {}: {}",
     MaxExpectedNv, ranges::distance(vs), view::all(vs));
    for (auto&& v : vs) { this->push_back(v); }
  }

  template <typename P,
            CONCEPT_REQUIRES_(
             Polygon<P>{} and !Range<P>{}
             and !Same<small_polygon<Nd, MaxExpectedNv>, uncvref_t<P>>{})>
  small_polygon(P&& p) noexcept : small_polygon(vertices(p)) {}
};

template <dim_t Nd, dim_t MaxExpectedNv>
bool operator==(small_polygon<Nd, MaxExpectedNv> const& a,
                small_polygon<Nd, MaxExpectedNv> const& b) noexcept {
  return equal(a, b);
}

template <dim_t Nd, dim_t MaxExpectedNv>
bool operator!=(small_polygon<Nd, MaxExpectedNv> const& a,
                small_polygon<Nd, MaxExpectedNv> const& b) noexcept {
  return !(a == b);
}

template <dim_t Nd, dim_t MaxExpectedNv>
constexpr auto vertex_capacity(
 small_polygon<Nd, MaxExpectedNv> const& p) noexcept {
  return p.capacity();
}

template <dim_t Nd, dim_t MaxExpectedNv>
constexpr auto vertex_size(small_polygon<Nd, MaxExpectedNv> const& p) noexcept {
  return p.size();
}

/// Polygon vertices
template <dim_t Nd, dim_t MaxExpectedNv>
constexpr auto vertices(small_polygon<Nd, MaxExpectedNv> p) noexcept {
  return p;
}

/// Polygon vertices
template <dim_t Nd, dim_t MaxExpectedNv>
constexpr auto vertex(small_polygon<Nd, MaxExpectedNv> p, dim_t v) noexcept {
  return p[v];
}

/// Vertex indices
template <dim_t Nd, dim_t MaxExpectedNv>
auto vertex_indices(small_polygon<Nd, MaxExpectedNv> const& p) noexcept {
  return view::iota(std::size_t{0}, p.size());
}

/// OStream operator
template <typename OStream, dim_t Nd, dim_t MaxExpectedNv>
OStream& operator<<(OStream& os,
                    small_polygon<Nd, MaxExpectedNv> const& p) noexcept {
  return write_to_ostream(os, p);
}

}  // namespace polygon_primitive

using polygon_primitive::small_polygon;

}  // namespace hm3::geometry
