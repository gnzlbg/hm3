#pragma once
/// \file
///
/// Minimum and maximum corners of a bounding box.
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace detail {

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

}  // namespace detail

namespace {
static constexpr auto const& x_min = static_const<detail::x_min_fn>::value;
static constexpr auto const& x_max = static_const<detail::x_max_fn>::value;
}  // namespace

}  // namespace discrete

using discrete::x_min;
using discrete::x_max;

}  // namespace geometry
}  // namespace hm3
