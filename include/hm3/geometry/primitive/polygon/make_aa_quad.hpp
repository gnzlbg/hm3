#pragma once
/// \file
///
/// Makes an axis-aligned quad
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/polygon/polygon.hpp>

namespace hm3::geometry {

namespace polygon_primitive {

/// Makes an axis-aligned quad spanned by \p xmin and \p xmax along the \p d
/// axis.
///
/// \pre The value of xmin(d) ~= xmax(d), that is, \p xmin and \p xmax must be
/// aligned at the plane of \p d .
constexpr quad<3> make_aa_quad(point<3> xmin, point<3> xmax, dim_t d) noexcept {
  HM3_ASSERT(geometry::approx(xmin(d), xmax(d)), "");

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

  return quad<3>({{{p0, p1, p2, p3}}});
}

}  // namespace polygon_primitive

using polygon_primitive::make_aa_quad;

}  // namespace hm3::geometry
