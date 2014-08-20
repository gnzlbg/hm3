#pragma once
/// \file point.hpp Point
#include <hm3/types.hpp>
#include <hm3/utility/matrix.hpp>
#include <hm3/utility/floating_point.hpp>

namespace hm3 {
namespace geometry {

/// Point
/// Must be int_t because Eigen3 is int_t
template <int_t nd> using point = dense::vector<num_t, nd, uint_t>;

/// Vector:
template <int_t nd> using vector = point<nd>;

/// Point dimension
template <int_t nd>
constexpr auto dimension(point<nd> const&)
 HM3_RETURNS(std::integral_constant<int_t, nd>{});

/// Vector length
template <int_t nd> constexpr auto length(vector<nd> const& p) -> num_t {
  num_t tmp = 0;
  for (sint_t d = 0; d != nd; ++d) {
    const num_t x_i = p(d);
    tmp += x_i * x_i;
  }
  return std::sqrt(tmp);
}

template <class OStream, int_t nd>
OStream& operator<<(OStream& o, point<nd> const& p) {
  o << "(";
  for (sint_t d = 0; d != nd; ++d) {
    o << p(d);
    if (d != nd - 1) { o << ", "; }
  }
  o << ")";
  return o;
}

}  // namespace geometry

namespace dense {

template <int_t nd>
bool operator==(geometry::point<nd> const& l, geometry::point<nd> const& r) {
  for (sint_t d = 0; d != nd; ++d) {
    if (!math::approx(l(d), r(d))) { return false; }
  }
  return true;
}

template <int_t nd>
bool operator!=(geometry::point<nd> const& l, geometry::point<nd> const& r) {
  return !(l == r);
}

}  // namespace dense
}  // namespace hm3
