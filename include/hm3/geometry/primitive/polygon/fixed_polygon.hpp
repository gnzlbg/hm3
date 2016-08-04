#pragma once
/// \file
///
/// A fixed polygon: contains a fixed number of vertices.
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/polygon/ostream.hpp>
#include <hm3/geometry/rank.hpp>
#include <hm3/utility/array.hpp>

namespace hm3::geometry {

/// Polygon primitives.
namespace polygon_primitive {

/// Bounded polygon
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam Nv Number of vertices.
template <dim_t Nd, dim_t Nv>
struct fixed_polygon : ranked<Nd, 2>, array<point<Nd>, Nv> {
  static_assert(Nv > 2, "");
  using vertex_index_type = dim_t;
  using face_index_type   = dim_t;

  using array_t = array<point<Nd>, Nv>;
  using array_t::array_t;
  using array_t::operator=;

  fixed_polygon() = default;

  template <typename Vertices, CONCEPT_REQUIRES_(Range<Vertices>{})>
  fixed_polygon(Vertices&& vs) noexcept {
    HM3_ASSERT(ranges::distance(vs) == Nv, "");
    ranges::copy(vs, begin(*this));
  }

  fixed_polygon(std::initializer_list<point<Nd>> vs) noexcept {
    HM3_ASSERT(ranges::distance(vs) == Nv, "");
    ranges::copy(vs, begin(*this));
  }
};

template <dim_t Nd, dim_t Nv>
bool operator==(fixed_polygon<Nd, Nv> const& a,
                fixed_polygon<Nd, Nv> const& b) noexcept {
  return equal(a, b);
}

template <dim_t Nd, dim_t Nv>
bool operator!=(fixed_polygon<Nd, Nv> const& a,
                fixed_polygon<Nd, Nv> const& b) noexcept {
  return !(a == b);
}

template <dim_t Nd, dim_t Nv>
constexpr auto vertex_capacity(fixed_polygon<Nd, Nv> const&) noexcept {
  return Nv;
}

template <dim_t Nd, dim_t Nv>
constexpr auto vertex_size(fixed_polygon<Nd, Nv> const&) noexcept {
  return Nv;
}

/// Vertices of the polygon \p p.
template <typename FixedPoly, typename UFP = uncvref_t<FixedPoly>,
          dim_t Nd = UFP::dimension(), dim_t Nv = UFP::size(),
          CONCEPT_REQUIRES_(Same<UFP, fixed_polygon<Nd, Nv>>{})>
constexpr decltype(auto) vertices(FixedPoly&& p) noexcept {
  return std::forward<FixedPoly>(p);
}

/// Vertex \p v of the polygon \p p.
template <typename FixedPoly, typename UFP = uncvref_t<FixedPoly>,
          dim_t Nd = UFP::dimension(), dim_t Nv = UFP::size(),
          CONCEPT_REQUIRES_(Same<UFP, fixed_polygon<Nd, Nv>>{})>
constexpr decltype(auto) vertex(FixedPoly&& p, dim_t v) noexcept {
  return std::forward<FixedPoly>(p)[v];
}

/// Vertex indices
template <dim_t Nd, dim_t Nv>
auto vertex_indices(fixed_polygon<Nd, Nv> const&) noexcept {
  return view::iota(std::size_t{0}, fixed_polygon<Nd, Nv>::size());
}

/// OStream operator
template <typename OStream, dim_t Nd, dim_t Nv>
OStream& operator<<(OStream& os, fixed_polygon<Nd, Nv> const& p) noexcept {
  return write_to_ostream(os, p);
}

}  // namespace polygon_primitive

using polygon_primitive::fixed_polygon;

}  // namespace hm3::geometry
