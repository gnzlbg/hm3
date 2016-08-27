#pragma once
/// \file
///
/// Are two points approximately equal?
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/primitive/point/point.hpp>

namespace hm3::geometry::point_primitive {

/// Computes if the points \p a and \p b are approximately equal using the
/// absolute and relative tolerances \p `abs_tol` and \p `rel_tol`,
/// respectively.
template <dim_t Nd>
constexpr auto approx(point<Nd> a, point<Nd> b, num_t abs_tol,
                      num_t rel_tol) noexcept {
  for (dim_t d = 0; d < Nd; ++d) {
    if (!geometry::approx(a(d), b(d), abs_tol, rel_tol)) { return false; }
  }
  return true;
}

}  // namespace hm3::geometry::point_primitive
