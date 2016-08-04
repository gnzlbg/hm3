#pragma once
/// \file
///
/// The relative position of one primitive with respect to another.
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

enum class relative_position_t { inside, outside, intersected };

namespace relative_position_detail {

relative_position_t invert(relative_position_t o) {
  switch (o) {
    case relative_position_t::inside: {
      return relative_position_t::outside;
    }
    case relative_position_t::outside: {
      return relative_position_t::inside;
    }
    case relative_position_t::intersected: {
      return relative_position_t::intersected;
    }
  }
}

struct relative_position_fn {
  template <typename T, typename U>
  static constexpr auto impl(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    invert(relative_position(std::forward<U>(u), std::forward<T>(t))));

  template <typename T, typename U>
  static constexpr auto impl(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(relative_position(std::forward<T>(t),
                                                          std::forward<U>(u)));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             std::forward<U>(u), 0));
};

}  // namespace relative_position_detail

namespace {
static constexpr auto const& relative_position
 = static_const<relative_position_detail::relative_position_fn>::value;
}  // namespace

}  // namespace hm3::geometry
