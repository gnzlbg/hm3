#pragma once
/// \file
///
/// Point type
#include <hm3/geometry/dimensions.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3 {
namespace geometry {

template <dim_t Nd>
using point_base_t = dense::vector<num_t, static_cast<int_t>(Nd), dim_t>;

template <dim_t Nd>  //
struct point : point_base_t<Nd>, dimensional<Nd> {
  using self = point_base_t<Nd>;
  using self::self;
  using self::operator=;
  constexpr point()   = default;
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
};

template <dim_t Nd>
bool operator==(point<Nd> const& l, point<Nd> const& r) noexcept {
  for (dim_t d = 0; d < Nd; ++d) {
    if (!math::approx(l(d), r(d))) { return false; }
  }
  return true;
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
  return !(l > r);
}

template <dim_t Nd>
bool operator>=(point<Nd> const& l, point<Nd> const& r) noexcept {
  return !(l < r);
}

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, point<Nd> const& p) {
  o << "(";
  for (dim_t d = 0; d < Nd; ++d) {
    o << p(d);
    if (d != Nd - 1) { o << ", "; }
  }
  o << ")";
  return o;
}

}  // namespace geometry
}  // namespace hm3
