#pragma once
/// \file
///
/// Split one primitive with another
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace split_detail {

struct split_fn {
  template <typename T, typename U>
  static constexpr auto split_impl(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(split(std::forward<U>(u),
                                              std::forward<T>(t)));

  template <typename T, typename U>
  static constexpr auto split_impl(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(split(std::forward<T>(t),
                                              std::forward<U>(u)));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(split_impl(std::forward<T>(t),
                                                   std::forward<U>(u), 0));
};

}  // namespace split_detail

namespace {
static constexpr auto const& split
 = static_const<split_detail::split_fn>::value;
}  // namespace

}  // namespace hm3::geometry
