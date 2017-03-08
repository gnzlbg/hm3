#pragma once
/// \file
///
/// Point type.
#include <hm3/geometry/primitive/vec.hpp>
#include <hm3/types.hpp>
#include <hm3/math/core.hpp>
#include <iomanip>

namespace hm3::geometry {

/// Point primitive.
namespace point_primitive {

/// Point.
///
/// \tparam Ad Ambient dimension.
template <dim_t Ad>  //
struct point : private point_vector_detail::base_t<Ad> {
  using geometry_type = trait::point<Ad>;
  using vector_t      = vec<Ad>;
  using base_t        = point_vector_detail::base_t<Ad>;
  using base_t::base_t;
  using base_t::operator=;
  using base_t::operator();
  using base_t::operator[];
  using base_t::c;
  using base_t::begin;
  using base_t::end;
  using base_t::zero;
  using base_t::ones;
  using typename base_t::iterator;
  using typename base_t::const_iterator;
  using typename base_t::reference;
  using typename base_t::const_reference;
  using typename base_t::value_type;

  constexpr point() = default;
  constexpr point(base_t const& s) : base_t(s) {}
  constexpr static point constant(num_t v) noexcept {
    return point{base_t::constant(v)};
  }
  constexpr static point unit(dim_t d) noexcept {
    HM3_ASSERT(
     d < Ad,
     "cannot create point with unit at {} for ambient dimension Ad = {}}", d,
     Ad);
    return point{base_t::unit(d)};
  }

  template <typename Rng, typename It = range_iterator_t<Rng>,
            CONCEPT_REQUIRES_(
             Range<Rng>{} and std::is_same<iterator_value_t<It>, point<Ad>>{})>
  constexpr point(Rng&& rng) : point{*ranges::begin(rng)} {
    HM3_ASSERT(ranges::distance(rng) == 1, "?");
  }

  operator base_t() const noexcept = delete;

  /// Explicit construction from associated vector type.
  explicit constexpr point(vector_t const& v) noexcept
   : base_t(reinterpret_cast<base_t const&>(v)) {}

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <dim_t Ad>
bool operator==(point<Ad> const& l, point<Ad> const& r) noexcept {
  return (l().array() == r().array()).all();
}

template <dim_t Ad>
bool operator!=(point<Ad> const& l, point<Ad> const& r) noexcept {
  return !(l == r);
}

template <dim_t Ad>
bool operator<(point<Ad> const& l, point<Ad> const& r) noexcept {
  return (l().array() < r().array()).all();
}

template <dim_t Ad>
bool operator>(point<Ad> const& l, point<Ad> const& r) noexcept {
  return (l().array() > r().array()).all();
}

template <dim_t Ad>
bool operator<=(point<Ad> const& l, point<Ad> const& r) noexcept {
  return (l().array() <= r().array()).all();
}

template <dim_t Ad>
bool operator>=(point<Ad> const& l, point<Ad> const& r) noexcept {
  return (l().array() >= r().array()).all();
}

/// \name VertexAccess
///@{

/// Number of vertices in a point
template <dim_t Ad>
constexpr dim_t vertex_size(point<Ad> const&) noexcept {
  return 1;
}

/// Vertex of a point (its the point itself).
template <dim_t Ad>
constexpr point<Ad> const& vertex(point<Ad> const& p, dim_t v) noexcept {
  HM3_ASSERT(v == 0, "");
  return p;
}

/// Vertex of a point (its the point itself).
template <dim_t Ad>
constexpr point<Ad> vertex(point<Ad>&& p, dim_t v) noexcept {
  HM3_ASSERT(v == 0, "");
  return p;
}

/// Vertex of a point (its the point itself).
template <dim_t Ad>
constexpr point<Ad>& vertex(point<Ad>& p, dim_t v) noexcept {
  HM3_ASSERT(v == 0, "");
  return p;
}

///@} // VertexAccess

}  // namespace point_primitive

using point_primitive::point;

/// TODO: move somewhere else
template <typename P, typename PT = uncvref_t<P>,
          typename VT = associated::vector_t<PT>>
constexpr VT as_v(P p) noexcept {
  return reinterpret_cast<VT&&>(p);
}

}  // namespace hm3::geometry
