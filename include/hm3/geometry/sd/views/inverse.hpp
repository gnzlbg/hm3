#pragma once
/// \file
///
/// Inverse of a signed distance field
#include <hm3/geometry/sd/core.hpp>

namespace hm3::geometry::sd {

namespace detail {
/// Inverse of a signed distance field
struct op_inverse_fn {
  template <typename Point, typename SDF>
  constexpr auto operator()(Point&& x, SDF&& f) const {
    return -f(std::forward<Point>(x));
  }
};
}  // namespace detail

namespace {
constexpr auto const& op_inverse = static_const<detail::op_inverse_fn>::value;
}  // namespace

namespace detail {
struct invert_fn {
  template <typename SDF>
  constexpr auto operator()(SDF&& f) const {
    return view(op_inverse, std::forward<SDF>(f));
  }
};
}  // namespace detail

namespace {
constexpr auto const& invert = static_const<detail::invert_fn>::value;
}  // namespace

}  // namespace hm3::geometry::sd
