#pragma once
/// \file
///
/// Intersection of lines.
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

  auto t = ps.value().first;
  auto u = ps.value().second;

  auto pia = a.x(t);
  auto pib = b.x(u);
  fmt::print("t: {}, a(t): {}, u: {}, b(u): {}\n", t, pia, u, pib);
  for (dim_t d = 0; d < 3; ++d) {
    if (!math::approx(pia(d), pib(d))) { return false; }
  }
  return true;
}

// template <dim_t Nd>
// bool intersect_at_point(line<Nd> const& a, line<Nd> const& b) {
//   HM3_ASSERT(!parallel(a, b), "");

//   auto a_o         = a.origin();
//   auto b_o         = b.origin();
//   auto a_d         = a.direction();
//   auto b_d         = b.direction();
//   auto bo_min_ao   = vec<Nd>(b_o() - a_o());
//   auto ad_cross_bd = perp_product_norm(a_d, b_d);

//   auto bo_min_ao_cross_bd = perp_product_norm(bo_min_ao, b_d);
//   auto t                  = bo_min_ao_cross_bd / ad_cross_bd;
//   auto bo_min_ao_cross_ad = perp_product_norm(bo_min_ao, a_d);
//   auto u                  = bo_min_ao_cross_ad / ad_cross_bd;

//   if (0. <= t and t <= 1. and 0. <= u and u <= 1.) {
//     return true;  // both lines intersect at a point
//   }
//   fmt::print("t: {}, u: {}\n", t, u);
//   return false;  // Case 3: not parallel and not intersect
// }

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

  auto t = ps.value().first;
  auto s = ps.value().second;

  auto a_p = a.x(t);
  auto b_p = b.x(s);
  fmt::print("t: {}, a(t): {}, u: {}, b(u): {}\n", t, a_p, s, b_p);
  if (distance.approx(distance.centroid(a_p, b_p), 0.)) { return a_p; }
  return monostate{};
}

}  // namespace hm3::geometry::line_primitive
