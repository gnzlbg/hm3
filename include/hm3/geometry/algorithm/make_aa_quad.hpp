#pragma once
/// \file
///
/// Makes an axis-aligned quad.
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/concept/polygon.hpp>

namespace hm3::geometry {

/// Makes an axis-aligned quad spanned by \p xmin and \p xmax along the \p d
/// axis.
///
/// \pre The value of xmin(d) ~= xmax(d), that is, \p xmin and \p xmax must be
/// aligned at the plane of \p d .
template <typename QT, typename PT>
constexpr QT make_aa_quad(PT xmin, PT xmax, dim_t d) noexcept {
  HM3_ASSERT(approx_number(xmin(d), xmax(d)), "");
  static_assert(ad_v<PT> == 3);
  static_assert(ad_v<QT> == 3);
  static_assert(Polygon<QT, 3>{});

  using e_t = associated::edge_t<QT>;

  // map non-equal component to the other two
  constexpr dim_t c0s[3] = {1, 2, 0};
  constexpr dim_t c1s[3] = {2, 0, 1};

  dim_t c0 = c0s[d];
  dim_t c1 = c1s[d];

  // compute the quad corner points
  auto p0 = xmin;
  auto p1 = xmin;
  p1(c1)  = xmax(c1);
  auto p2 = xmax;
  auto p3 = xmin;
  p3(c0)  = xmax(c0);

  auto es = {e_t{p0, p1}, e_t{p1, p2}, e_t{p2, p3}, e_t{p3, p0}};
  return QT(es);
}

}  // namespace hm3::geometry
