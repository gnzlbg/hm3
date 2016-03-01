#pragma once
/// \file
///
/// Linear interpolation in n spatial dimensions: two points
#include <hm3/geometry/line.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/interpolation/concept.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/config/assert.hpp>

namespace hm3 {
namespace ip {

/// Linear interpolation utilities
namespace linear {

using geometry::point;
using geometry::line;

/// Given two point value pairs, (x0, v0) and (x1, v1), interpolates value at x
///
/// Equation: y(x) = y0 + (y1 - y0) / ||x1 - x0|| * ||x - x0||
///
/// Note: x and y can be multi-dimensional
template <uint_t Nd, typename T>
T interpolate(point<Nd> x, point<Nd> x0, point<Nd> x1, T v0, T v1) noexcept {
  HM3_ASSERT((x1() - x0()).norm() > 0.,
             "points x0 and x1 must be different! x0: {}, x1: {}", x0, x1);
  return v0 + (v1 - v0) * (x() - x0()).norm() / (x1() - x0()).norm();
}

template <uint_t Nd, typename T>
T interpolate(num_t d, line<Nd> l, T v0, T v1) {
  HM3_ASSERT((l.x_1() - l.x_0()).norm() > 0.,
             "points x0 and x1 must be different! x0: {}, x1: {}", l.x_0,
             l.x_1);
  return v0 + (v1 - v0) * d / (l.x_1() - l.x_0()).norm();
}

template <uint_t Nd, typename T>
T interpolate(num_t d, line<Nd> l, std::array<T, 2> vs) {
  return interpolate(d, l, vs[0], vs[1]);
}

template <uint_t Nd, typename T>
num_t distance_to_value(T v, point<Nd> x0, point<Nd> x1, T v0, T v1) {
  HM3_ASSERT(v1 - v0 > 0. or v1 - v0 < 0., "");
  return (x1() - x0()).norm() * (v - v0) / (v1 - v0);  // TODO: use .array()
}

template <uint_t Nd, typename T>
num_t distance_to_value(T v, line<Nd> l, std::array<T, 2> vs) {
  return distance_to_value(v, l.x_0, l.x_1, vs[0], vs[1]);
}

template <uint_t Nd, typename T>
point<Nd> point_with_value(T v, point<Nd> x0, point<Nd> x1, T v0, T v1) {
  auto dir = direction(line<Nd>::through(x0, x1));
  return point<Nd>{x0() + dir() * distance_to_value(v, x0, x1, v0, v1)};
}

template <uint_t Nd, typename T>
point<Nd> point_with_value(T v, line<Nd> l, std::array<T, 2> vs) {
  return point_with_value(v, l.x_0, l.x_1, vs[0], vs[1]);
}

template <typename D, CONCEPT_REQUIRES_(Interpolable<D>{})>
void assert_is_valid_dataset(D const& d) {
  HM3_ASSERT(size(d.points()) == 2,
             "linear interpolation requires 2 points but {} points provided",
             size(d.points()));
  HM3_ASSERT(size(d.values()) == 2,
             "linear interpolation requires 2 values but {} values provided",
             size(d.values()));
}

template <uint_t Nd, typename D, CONCEPT_REQUIRES_(Interpolable<D>{})>
auto interpolate(point<Nd> const& x, D const& d) {
  assert_is_valid_dataset(d);
  return interpolate(x, d.points()[0], d.points()[1], d.values()[0],
                     d.values()[1]);
}

}  // namespace linear
}  // namespace ip
}  // namespace hm3
