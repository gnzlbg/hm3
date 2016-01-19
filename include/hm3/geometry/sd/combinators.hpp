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
auto inverse(Point&& x, SDFunction&& f) {
  return -f(x);
}

/// Union of a signed distance field
template <typename Point, typename SDFunction1, typename SDFunction2>
auto union_(Point&& x, SDFunction1&& f1, SDFunction2&& f2) {
  return std::min(f1(x), f2(x));
}

/// Union of a signed distance field
template <typename Point, typename SDFunction1, typename... SDFunctions>
auto union_(Point&& x, SDFunction1&& f1, SDFunctions&&... fs) {
  return union_(f1, union_(x, fs...));
}

/// Intersection of a signed distance field
template <typename Point, typename SDFunction1, typename SDFunction2>
auto intersection(Point&& x, SDFunction1& f1, SDFunction2& f2) {
  return std::max(f1(x), f2(x));
}

/// Intersection of a signed distance field
template <typename Point, typename SDFunction1, typename... SDFunctions>
auto intersection(Point&& x, SDFunction1&& f1, SDFunctions&&... fs) {
  return intersection(x, f1, intersection(x, fs...));
}

/// Difference of a signed distance field
template <typename Point, typename SDFunction1, typename SDFunction2>
auto difference(Point&& x, SDFunction1& f1, SDFunction2& f2) {
  return std::max(f1(x), -f2(x));
}

// TODO: how to move a parameter pack into a lambda?
// TODO: constrain for rvalues only
// template <typename SDAdaptor, typename... SDFunctions>
// auto move_adapt(SDAdaptor a, SDFunctions&&... sdfs) {
//   return [ ma = std::move(a), (msdfs = std::move(sdfs))... ](auto&& x) {
//     return ma(x, msdfs...);
//   };
// }

/// Inverts a signed distance field
template <typename SDFunction> auto invert(SDFunction&& f) {
  return [&](auto&& x) { return inverse(x, std::forward<SDFunction>(f)); };
}

}  // namespace sd
}  // namespace geometry
}  // namespace hm3
