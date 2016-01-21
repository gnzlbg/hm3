#pragma once
/// \file
///
/// Combine signed distance functions into new ones
///
/// TODO: make combinators function objects
#include <hm3/types.hpp>

namespace hm3 {
namespace geometry {
namespace sd {

/// Inverse of a signed distance field
template <typename Point, typename SDFunction>
auto op_inverse(Point&& x, SDFunction&& f) {
  return -f(x);
}

/// Union of a signed distance field: OR(f1, f2)
template <typename Point, typename SDFunction1, typename SDFunction2>
auto op_union(Point&& x, SDFunction1&& f1, SDFunction2&& f2) {
  return std::min(f1(x), f2(x));
}

/// Union of a signed distance field: OR(f1, f2)
template <typename Point, typename SDFunction1, typename... SDFunctions>
auto op_union(Point&& x, SDFunction1&& f1, SDFunctions&&... fs) {
  return op_union(f1, op_union(x, fs...));
}

/// Intersection of a signed distance field: AND(f1, f2)
template <typename Point, typename SDFunction1, typename SDFunction2>
auto op_intersection(Point&& x, SDFunction1& f1, SDFunction2& f2) {
  return std::max(f1(x), f2(x));
}

/// Intersection of a signed distance field: AND(f1, f2)
template <typename Point, typename SDFunction1, typename... SDFunctions>
auto op_intersection(Point&& x, SDFunction1&& f1, SDFunctions&&... fs) {
  return op_intersection(x, f1, op_intersection(x, fs...));
}

/// Difference of a signed distance field: AND(f1, -f2)
template <typename Point, typename SDFunction1, typename SDFunction2>
auto op_difference(Point&& x, SDFunction1& f1, SDFunction2& f2) {
  return std::max(f1(x), -f2(x));
}

}  // namespace sd
}  // namespace geometry
}  // namespace hm3
