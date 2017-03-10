#pragma once
/// \file
///
/// Minimum and maximum corners of a bounding box.
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace x_min_max_detail {

struct x_min_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(x_min(std::forward<T>(t)));
};

struct x_max_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(x_max(std::forward<T>(t)));
};

}  // namespace x_min_max_detail

namespace {
constexpr auto const& x_min = static_const<x_min_max_detail::x_min_fn>::value;
constexpr auto const& x_max = static_const<x_min_max_detail::x_max_fn>::value;
}  // namespace

}  // namespace hm3::geometry
