#pragma once
/// \file
///
/// Are two vectors approximately equal?
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/primitive/point/approx.hpp>
#include <hm3/geometry/primitive/vec/vec.hpp>

namespace hm3::geometry::vec_primitive {

/// Computes if the vectors \p a and \p b are approximately equal using the
/// absolute and relative tolerances \p `abs_tol` and \p `rel_tol`,
/// respectively.
template <dim_t Nd>
constexpr bool approx(vec<Nd> a, vec<Nd> b, num_t abs_tol,
                      num_t rel_tol) noexcept {
  return geometry::approx(a.as_point(), b.as_point(), abs_tol, rel_tol);
}

}  // namespace hm3::geometry::vec_primitive
