#pragma once
/// \file
///
/// Combine signed distance functions into new ones
#include <hm3/types.hpp>

namespace hm3 {
namespace geometry {
namespace sd {

/// Inverts a signed distance field
template <typename SDFunction> auto invert(SDFunction&& f) {
  return [&](auto&& x) { return -f(x); };
}

template <typename SDFunction> struct inverted : SDFunction {
  using SDFunction::SDFunction;
  using SDFunction::operator=;

  template <typename... Args> num_t operator()(Args&&... args) const noexcept {
    return invert(SDFunction::operator()(std::forward<Args>(args)...));
  }
  template <typename... Args> num_t operator()(Args&&... args) noexcept {
    return invert(SDFunction::operator()(std::forward<Args>(args)...));
  }
};

template <typename SDFunction>
constexpr auto dimension(inverted<SDFunction> const&) noexcept {
  return dimension(SDFunction());
}

/// Union of a signed distance field
template <typename SDFunction1, typename SDFunction2>
auto take_union(SDFunction1&& f1, SDFunction2&& f2) {
  return [=](auto&& x) { return std::min(f1(x), f2(x)); };
}

/// Union of a signed distance field
template <typename SDFunction1, typename... SDFunctions>
auto take_union(SDFunction1&& f1, SDFunctions&&... fs) {
  return take_union(f1, take_union(fs...));
}

/// Intersection of a signed distance field
template <typename SDFunction1, typename SDFunction2>
auto take_intersection(SDFunction1& f1, SDFunction2& f2) {
  return [&](auto&& x) { return std::max(f1(x), f2(x)); };
}

/// Difference of a signed distance field
template <typename SDFunction1, typename SDFunction2>
auto take_difference(SDFunction1& f1, SDFunction2& f2) {
  return [&](auto&& x) { return std::max(f1(x), -f2(x)); };
}

}  // namespace sd
}  // namespace geometry
}  // namespace hm3
