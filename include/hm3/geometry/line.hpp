#pragma once
/// \file
#include <hm3/geometry/point.hpp>

namespace hm3 {
namespace geometry {

template <uint_t Nd> struct line {
  point<Nd> x_0, x_1;
  /// Line that goes through the 2 points \p b and \p e
  static line<Nd> through(point<Nd> b, point<Nd> e) noexcept {
    return line<Nd>{b, e};
  }
  auto operator()() const noexcept {
    return Eigen::ParametrizedLine<num_t, Nd>::Through(x_0(), x_1());
  }
};

/// Line dimension
template <uint_t Nd>
constexpr std::integral_constant<uint_t, Nd> dimension(line<Nd> const&) {
  return std::integral_constant<uint_t, Nd>{};
}

/// Line length
template <uint_t Nd> constexpr num_t length(line<Nd> const& l) {
  return (l.x_1() - l.x_0()).norm();
}

/// Line centroid
template <uint_t Nd> constexpr point<Nd> centroid(line<Nd> const& l) {
  point<Nd> v;
  v() = l.x_0() + 0.5 * (l.x_1() - l.x_0());
  return v;
}

/// Line direction
template <uint_t Nd> constexpr point<Nd> direction(line<Nd> const& l) {
  point<Nd> v;
  v = (l.x_1() - l.x_0()) / (l.x_1() - l.x_0()).norm();
  return v;
}

/// Line counter clockwise normal vector
template <uint_t Nd, CONCEPT_REQUIRES_(Nd == 2)>
constexpr point<Nd> normal(line<Nd> const& l) {
  num_t dx  = l.x_1(0) - l.x_0(0);
  num_t dy  = l.x_1(1) - l.x_0(1);
  num_t len = length(l);
  point<Nd> n;
  n(0) = -dy / len;
  n(1) = dx / len;
  return n;
}

template <uint_t Nd> constexpr line<Nd> flip(line<Nd> l) {
  using std::swap;
  swap(l.x_0, l.x_1);
  return l;
}

template <typename OStream, uint_t Nd>
OStream& operator<<(OStream& o, line<Nd> const& l) {
  o << "(x_0{";
  for (sint_t d = 0; d != Nd; ++d) {
    o << l.x_0(d);
    if (d != Nd - 1) { o << ", "; }
  }
  o << "}, x_1{";
  for (sint_t d = 0; d != Nd; ++d) {
    o << l.x_1(d);
    if (d != Nd - 1) { o << ", "; }
  }
  o << "})";
  return o;
}

}  // namespace geometry
}  // namespace hm3
