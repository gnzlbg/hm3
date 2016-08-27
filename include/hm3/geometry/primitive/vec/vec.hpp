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
template <dim_t Nd>
struct vec : private point_primitive::point_base_t<Nd>, public ranked<Nd, 1> {
  using self              = point_primitive::point_base_t<Nd>;
  using vertex_index_type = dim_t;
  using self::self;
  using self::operator=;
  using self::operator();
  using self::c;
  using self::begin;
  using self::end;
  using self::zero;
  using self::ones;
  using typename self::iterator;
  using typename self::const_iterator;
  using typename self::reference;
  using typename self::const_reference;
  using typename self::value_type;

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

  auto normalized() const noexcept { return vec<Nd>((*this)().normalized()); }

  operator self() const noexcept = delete;

  explicit vec(point<Nd> const& p) noexcept {
    reinterpret_cast<self&>(*this) = reinterpret_cast<self const&>(p);
  }

  point<Nd> as_point() const noexcept {
    return reinterpret_cast<self const&>(*this);
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <dim_t Nd>
bool operator==(vec<Nd> const& l, vec<Nd> const& r) noexcept {
  return (l().array() == r().array()).all();
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

}  // namespace vec_primitive

using vec_primitive::vec;

}  // namespace hm3::geometry
