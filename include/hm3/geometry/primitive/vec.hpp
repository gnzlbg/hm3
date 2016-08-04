#pragma once
/// \file
///
/// Vector primitive
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/primitive/point/point.hpp>

namespace hm3::geometry {

/// Vector primitive.
namespace vec_primitive {

/// Nd-dimensional Vec.
///
/// \tparam Nd Number of spatial dimensions.
template <dim_t Nd>  //
struct vec : point_primitive::point_base_t<Nd>, ranked<Nd, 1> {
  using self              = point_primitive::point_base_t<Nd>;
  using vertex_index_type = dim_t;
  using self::self;
  using self::operator=;

  constexpr vec() = default;
  constexpr vec(self const& s) : self(s) {}
  constexpr static vec constant(num_t v) noexcept {
    return vec{self::constant(v)};
  }
  constexpr static vec unit(dim_t d) noexcept {
    HM3_ASSERT(d < Nd,
               "cannot create point with unit at {} for {}-spatial-dimension",
               d, Nd);
    return vec{self::unit(d)};
  }

  template <typename Rng, typename It = range_iterator_t<Rng>,
            CONCEPT_REQUIRES_(
             Range<Rng>{} and std::is_same<iterator_value_t<It>, vec<Nd>>{})>
  constexpr vec(Rng&& rng) : vec{*begin(rng)} {
    HM3_ASSERT(ranges::distance(rng) == 1, "?");
  }

  explicit vec(point<Nd> const& p) noexcept {
    static_cast<self&>(*this) = static_cast<self const&>(p);
  }

  explicit operator point<Nd>() const noexcept {
    return point<Nd>(static_cast<self const&>(*this));
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/// Length of the vec.
template <dim_t Nd>
constexpr num_t integral(vec<Nd> const& v, path_integral<vec<Nd>>) noexcept {
  return v().norm();
}

/// Direction of the vec (the vec itself).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Same<UV, vec<Nd>>{})>
constexpr decltype(auto) direction(V&& v) noexcept {
  return v;
}

/// Normal to the vec (2D only).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 2 and Same<UV, vec<Nd>>{})>
constexpr UV normal(V&& v) noexcept {
  return UV{-v(1), v(0)};
}

/// Vec cross-product (3D only).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 3 and Same<UV, vec<Nd>>{})>
constexpr UV cross(V const& v0, V const& v1) noexcept {
  return UV{v0().cross(v1())};
}

/// Vec cross-product (2D only, returns a 3D vector).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 2 and Same<UV, vec<Nd>>{})>
constexpr auto cross(V const& v0, V const& v1) noexcept {
  vec<3> v0_{v0(0), v0(1), 0.};
  vec<3> v1_{v1(0), v1(1), 0.};
  return cross(v0_, v1_);
}

template <dim_t Nd>
bool operator==(vec<Nd> const& l, vec<Nd> const& r) noexcept {
  return (l().array() == r().array()).all();
}

template <dim_t Nd>
bool approx(vec<Nd> const& l, vec<Nd> const& r, num_t tol = 1e-14) noexcept {
  return (l().array() - r().array()).abs().sum() < tol;
}

template <dim_t Nd>
bool operator!=(vec<Nd> const& l, vec<Nd> const& r) noexcept {
  return !(l == r);
}

template <dim_t Nd>
bool operator<(vec<Nd> const& l, vec<Nd> const& r) noexcept {
  return (l().array() < r().array()).all();
}

template <dim_t Nd>
bool operator>(vec<Nd> const& l, vec<Nd> const& r) noexcept {
  return (l().array() > r().array()).all();
}

template <dim_t Nd>
bool operator<=(vec<Nd> const& l, vec<Nd> const& r) noexcept {
  return (l().array() <= r().array()).all();
}

template <dim_t Nd>
bool operator>=(vec<Nd> const& l, vec<Nd> const& r) noexcept {
  return (l().array() >= r().array()).all();
}

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, vec<Nd> const& p) {
  o << "(";
  for (dim_t d = 0; d < Nd; ++d) {
    o << std::setprecision(std::numeric_limits<num_t>::digits10 + 1) << p(d);
    if (d != Nd - 1) { o << ", "; }
  }
  o << ")";
  return o;
}

}  // namespace vec_primitive

using vec_primitive::vec;

}  // namespace hm3::geometry