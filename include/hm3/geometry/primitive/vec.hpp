#pragma once
/// \file
///
/// Vector primitive
#include <hm3/geometry/fwd.hpp>
#include <hm3/geometry/primitive/detail/point_vector_base.hpp>
#include <hm3/types.hpp>

namespace hm3::geometry {

/// Vector primitive.
namespace vec_primitive {

/// Ad-dimensional Vec.
///
/// \tparam Ad Number of ambient dimensions.
template <dim_t Ad>
struct vec : private point_vector_detail::base_t<Ad> {
  using geometry_type     = trait::vector<Ad>;
  using base_t            = point_vector_detail::base_t<Ad>;
  using vertex_index_type = dim_t;
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

  template <dim_t OtherAd>
  using rebind = vec<OtherAd>;

  constexpr vec() = default;
  explicit constexpr vec(base_t const& s) : base_t(s) {}
  constexpr static vec constant(num_t v) noexcept {
    return vec{base_t::constant(v)};
  }
  constexpr static vec unit(dim_t d) noexcept {
    HM3_ASSERT(d < Ad,
               "cannot create vector with unit at {} for {}-ambient-dimension",
               d, Ad);
    return vec{base_t::unit(d)};
  }

  template <typename Rng, typename It = range_iterator_t<Rng>,
            CONCEPT_REQUIRES_(
             Range<Rng>{} and std::is_same<iterator_value_t<It>, vec<Ad>>{})>
  explicit constexpr vec(Rng&& rng) : vec{*begin(rng)} {
    HM3_ASSERT(ranges::distance(rng) == 1, "?");
  }

  auto normalized() const noexcept { return vec<Ad>((*this)().normalized()); }

  // TODO: clang-tidy bug
  operator base_t() const noexcept  // NOLINT(google-explicit-constructor)
   = delete;

  base_t& layout() noexcept { return static_cast<base_t&>(*this); }
  base_t const& layout() const noexcept {
    return static_cast<base_t const&>(*this);
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <dim_t Ad>
bool operator==(vec<Ad> const& l, vec<Ad> const& r) noexcept {
  return (l().array() == r().array()).all();
}

template <dim_t Ad>
bool operator!=(vec<Ad> const& l, vec<Ad> const& r) noexcept {
  return !(l == r);
}

template <dim_t Ad>
bool operator<(vec<Ad> const& l, vec<Ad> const& r) noexcept {
  return (l().array() < r().array()).all();
}

template <dim_t Ad>
bool operator>(vec<Ad> const& l, vec<Ad> const& r) noexcept {
  return (l().array() > r().array()).all();
}

template <dim_t Ad>
bool operator<=(vec<Ad> const& l, vec<Ad> const& r) noexcept {
  return (l().array() <= r().array()).all();
}

template <dim_t Ad>
bool operator>=(vec<Ad> const& l, vec<Ad> const& r) noexcept {
  return (l().array() >= r().array()).all();
}

}  // namespace vec_primitive

using vec_primitive::vec;

}  // namespace hm3::geometry
