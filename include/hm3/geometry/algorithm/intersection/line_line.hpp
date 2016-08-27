#pragma once
/// \file
///
/// Intersection of lines.
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/primitive/line.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::line_primitive {

namespace intersection_detail {

constexpr bool intersect_at_point(line<1> const&, line<1> const&) noexcept {
  return false;
}

constexpr bool intersect_at_point(line<2> const& a, line<2> const& b) noexcept {
  return !parallel(a, b);
}

inline bool intersect_at_point(line<3> const& a, line<3> const& b) noexcept {
  if (parallel(a, b)) { return false; }

  auto ps = parameter(a, b);
  if (!ps) { return false; }

  auto t = ps.value().first();
  auto u = ps.value().second();

  auto pia = a.x(t);
  auto pib = b.x(u);
  fmt::print("t: {}, a(t): {}, u: {}, b(u): {}\n", t, pia, u, pib);
  for (dim_t d = 0; d < 3; ++d) {
    if (!math::approx(pia(d), pib(d))) { return false; }
  }
  return true;
}

}  // namespace intersection_detail

/// Do the lines \p a and \p b intersect?
template <dim_t Nd>
inline bool intersection_test(line<Nd> const& a, line<Nd> const& b) noexcept {
  if (parallel(a, b)) {
    if (coincidental(a, b)) { return true; }
    return false;
  }
  if (intersection_detail::intersect_at_point(a, b)) { return true; }
  return false;
}

/// Intersection between the lines \p a and \p b.
///
/// \returns Intersection. That is, either empty, point, or line.
template <dim_t Nd>
inline variant<monostate, point<Nd>, line<Nd>> intersection(
 line<Nd> const& a, line<Nd> const& b) noexcept {
  if (coincidental(a, b)) { return a; }
  auto ps = parameter(a, b);
  if (!ps) { return monostate{}; }

  auto t = ps.value().first();
  auto s = ps.value().second();

  auto a_p = a.x(t);
  auto b_p = b.x(s);
  fmt::print("t: {}, a(t): {}, u: {}, b(u): {}\n", t, a_p, s, b_p);
  if (approx(distance.centroid(a_p, b_p), 0.)) { return a_p; }
  return monostate{};
}

}  // namespace hm3::geometry::line_primitive
