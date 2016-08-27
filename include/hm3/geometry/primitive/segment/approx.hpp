#pragma once
/// \file
///
/// Are two segments approximately equal?
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/primitive/point/approx.hpp>

namespace hm3::geometry::segment_primitive {

/// Computes if the segmentes \p a and \p b are approximately equal using the
/// absolute and relative tolerances \p `abs_tol` and \p `rel_tol`,
/// respectively.
template <dim_t Nd>
constexpr bool approx(segment<Nd> a, segment<Nd> b, num_t abs_tol,
                      num_t rel_tol) noexcept {
  return geometry::approx(a.x(0), b.x(0), abs_tol, rel_tol)
         and geometry::approx(a.x(1), b.x(1), abs_tol, rel_tol);
}

}  // namespace hm3::geometry::segment_primitive
