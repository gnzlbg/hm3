#pragma once
/// \file
///
/// Difference of two signed-distance fields.
#include <hm3/geometry/sd/core.hpp>

namespace hm3::geometry::sd {

namespace detail {

struct op_difference_fn {
  /// Difference of a signed distance field: AND(f1, -f2)
  template <typename Point, typename SDF1, typename SDF2>
  constexpr auto operator()(Point&& x, SDF1&& f1, SDF2&& f2) const {
    return std::max(f1(x), -f2(x));
  }
};
}  // namespace detail

namespace {
constexpr auto const& op_difference
 = static_const<detail::op_difference_fn>::value;
}  // namespace

namespace detail {
struct difference_fn {
  template <typename SDF0, typename SDF1>
  constexpr auto operator()(SDF0&& f0, SDF1&& f1) const {
    return view(op_difference, std::forward<SDF0>(f0), std::forward<SDF1>(f1));
  }
};
}  // namespace detail

namespace {
constexpr auto const& difference = static_const<detail::difference_fn>::value;
}  // namespace

}  // namespace hm3::geometry::sd
