#pragma once
/// \file
///
/// Point type
#include <hm3/geometry/rank.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/matrix.hpp>
#include <iomanip>

namespace hm3::geometry {

/// Point primitive.
namespace point_primitive {

template <dim_t Nd>
using point_base_t = dense::vector<num_t, static_cast<int_t>(Nd), dim_t>;

/// Nd-dimensional point.
///
/// \tparam Nd Number of spatial dimensions.
template <dim_t Nd>  //
struct point : point_base_t<Nd>, ranked<Nd, 0> {
  using self              = point_base_t<Nd>;
  using vertex_index_type = dim_t;
  using self::self;
  using self::operator=;

  constexpr point() = default;
  constexpr point(self const& s) : self(s) {}
  constexpr static point constant(num_t v) noexcept {
    return point{point_base_t<Nd>::constant(v)};
  }
  constexpr static point unit(dim_t d) noexcept {
    HM3_ASSERT(d < Nd,
               "cannot create point with unit at {} for {}-spatial-dimension",
               d, Nd);
    return point{point_base_t<Nd>::unit(d)};
  }

  template <typename Rng, typename It = range_iterator_t<Rng>,
            CONCEPT_REQUIRES_(
             Range<Rng>{} and std::is_same<iterator_value_t<It>, point<Nd>>{})>
  constexpr point(Rng&& rng) : point{*begin(rng)} {
    HM3_ASSERT(ranges::distance(rng) == 1, "?");
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <dim_t Nd>
bool operator==(point<Nd> const& l, point<Nd> const& r) noexcept {
  return (l().array() == r().array()).all();
}

template <dim_t Nd>
bool approx(point<Nd> const& l, point<Nd> const& r,
            num_t tol = 1e-14) noexcept {
  return (l().array() - r().array()).abs().sum() < tol;
}

template <dim_t Nd>
bool operator!=(point<Nd> const& l, point<Nd> const& r) noexcept {
  return !(l == r);
}

template <dim_t Nd>
bool operator<(point<Nd> const& l, point<Nd> const& r) noexcept {
  return (l().array() < r().array()).all();
}

template <dim_t Nd>
bool operator>(point<Nd> const& l, point<Nd> const& r) noexcept {
  return (l().array() > r().array()).all();
}

template <dim_t Nd>
bool operator<=(point<Nd> const& l, point<Nd> const& r) noexcept {
  return (l().array() <= r().array()).all();
}

template <dim_t Nd>
bool operator>=(point<Nd> const& l, point<Nd> const& r) noexcept {
  return (l().array() >= r().array()).all();
}

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, point<Nd> const& p) {
  o << "(";
  for (dim_t d = 0; d < Nd; ++d) {
    o << std::setprecision(std::numeric_limits<num_t>::digits10 + 1) << p(d);
    if (d != Nd - 1) { o << ", "; }
  }
  o << ")";
  return o;
}

}  // namespace point_primitive

using point_primitive::point;

}  // namespace hm3::geometry
