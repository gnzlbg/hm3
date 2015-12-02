#pragma once
/// \file
///
/// Point
#include <hm3/types.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3 {
namespace geometry {

/// Point
/// Must be int_t because Eigen3 is int_t
template <int_t Nd> using point = dense::vector<num_t, Nd, uint_t>;

/// Vector:
template <int_t Nd> using vector = point<Nd>;

/// Point dimension
template <int_t Nd> constexpr auto dimension(point<Nd> const&) {
  return std::integral_constant<int_t, Nd>{};
}

/// Vector length
template <int_t Nd> constexpr auto length(vector<Nd> const& p) -> num_t {
  num_t tmp = 0;
  for (sint_t d = 0; d != Nd; ++d) {
    const num_t x_i = p(d);
    tmp += x_i * x_i;
  }
  return std::sqrt(tmp);
}

template <typename OStream, int_t Nd>
OStream& operator<<(OStream& o, point<Nd> const& p) {
  o << "(";
  for (sint_t d = 0; d != Nd; ++d) {
    o << p(d);
    if (d != Nd - 1) { o << ", "; }
  }
  o << ")";
  return o;
}

}  // namespace geometry

namespace dense {

template <int_t Nd>
bool operator==(geometry::point<Nd> const& l, geometry::point<Nd> const& r) {
  for (sint_t d = 0; d != Nd; ++d) {
    if (!math::approx(l(d), r(d))) { return false; }
  }
  return true;
}

template <int_t Nd>
bool operator!=(geometry::point<Nd> const& l, geometry::point<Nd> const& r) {
  return !(l == r);
}

}  // namespace dense
}  // namespace hm3
