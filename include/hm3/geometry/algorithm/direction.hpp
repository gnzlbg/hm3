#pragma once
/// \file
///
/// Direction
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace direction_detail {

struct direction_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(direction(std::forward<T>(t)));

  template <typename T>
  static constexpr auto invert(T&& t)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(invert_direction(std::forward<T>(t)));
};

}  // namespace direction_detail

namespace {
static constexpr auto const& direction
 = static_const<direction_detail::direction_fn>::value;
}  // namespace

}  // namespace hm3::geometry
