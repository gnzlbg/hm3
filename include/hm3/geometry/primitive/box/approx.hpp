#pragma once
/// \file
///
/// Are two boxes approximately equal?
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/primitive/box/box.hpp>
#include <hm3/geometry/primitive/point/approx.hpp>

namespace hm3::geometry::box_primitive {

/// Computes if the boxes \p a and \p b are approximately equal using the
/// absolute and relative tolerances \p `abs_tol` and \p `rel_tol`,
/// respectively.
template <dim_t Nd>
constexpr bool approx(box<Nd> a, box<Nd> b, num_t abs_tol,
                      num_t rel_tol) noexcept {
  return geometry::approx(a.centroid_, b.centroid_, abs_tol, rel_tol)
         and geometry::approx(a.length_, b.length_, abs_tol, rel_tol);
}

}  // namespace hm3::geometry::box_primitive
