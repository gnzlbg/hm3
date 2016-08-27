#pragma once
/// \file
///
/// Are two AABBs approximately equal?
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
#include <hm3/geometry/primitive/point/approx.hpp>

namespace hm3::geometry::aabb_primitive {

/// Computes if the AABBs \p a and \p b are approximately equal using the
/// absolute and relative tolerances \p `abs_tol` and \p `rel_tol`,
/// respectively.
template <dim_t Nd>
constexpr bool approx(aabb<Nd> a, aabb<Nd> b, num_t abs_tol,
                      num_t rel_tol) noexcept {
  return geometry::approx(x_min(a), x_min(b), abs_tol, rel_tol)
         and geometry::approx(x_max(a), x_max(b), abs_tol, rel_tol);
}

}  // namespace hm3::geometry::aabb_primitive
