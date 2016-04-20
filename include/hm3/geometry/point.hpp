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

template <uint_t Nd>
using point_base_t = dense::vector<num_t, static_cast<int_t>(Nd), suint_t>;

template <uint_t Nd>  //
struct point : point_base_t<Nd>, dimensional<Nd> {
  using self = point_base_t<Nd>;
  using self::self;
  using self::operator=;
  constexpr point()   = default;
  constexpr point(self const& s) : self(s) {}
  constexpr static point constant(num_t v) noexcept {
    return point{point_base_t<Nd>::constant(v)};
  }
  constexpr static point unit(suint_t d) noexcept {
    HM3_ASSERT(d < Nd,
               "cannot create point with unit at {} for {}-spatial-dimension",
               d, Nd);
    return point{point_base_t<Nd>::unit(d)};
  }
};

template <uint_t Nd>
bool operator==(point<Nd> const& l, point<Nd> const& r) noexcept {
  for (suint_t d = 0; d < Nd; ++d) {
    if (!math::approx(l(d), r(d))) { return false; }
  }
  return true;
}

template <uint_t Nd>
bool operator!=(point<Nd> const& l, point<Nd> const& r) noexcept {
  return !(l == r);
}

template <uint_t Nd>
bool operator<(point<Nd> const& l, point<Nd> const& r) noexcept {
  return (l().array() < r().array()).all();
}

template <uint_t Nd>
bool operator>(point<Nd> const& l, point<Nd> const& r) noexcept {
  return (l().array() > r().array()).all();
}

/// Distance between two points
template <uint_t Nd>
num_t distance(point<Nd> const& a, point<Nd> const& b) noexcept {
  return (b() - a()).norm();
}

template <typename OStream, uint_t Nd>
OStream& operator<<(OStream& o, point<Nd> const& p) {
  o << "(";
  for (suint_t d = 0; d != Nd; ++d) {
    o << p(d);
    if (d != Nd - 1) { o << ", "; }
  }
  o << ")";
  return o;
}

template <uint_t Nd> using vector = point<Nd>;

/// Vector length
template <uint_t Nd> constexpr num_t length(vector<Nd> const& p) {
  return p().norm();
}

}  // namespace geometry
}  // namespace hm3
