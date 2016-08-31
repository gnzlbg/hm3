#pragma once
/// \file
///
/// Split one primitive with another
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace sd_split_detail {

struct sd_split_fn {
  template <typename T, typename U>
  static constexpr auto sd_split_impl(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(sd_split(std::forward<U>(u),
                                                 std::forward<T>(t)));

  template <typename T, typename U>
  static constexpr auto sd_split_impl(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(sd_split(std::forward<T>(t),
                                                 std::forward<U>(u)));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(sd_split_impl(std::forward<T>(t),
                                                      std::forward<U>(u), 0));
};

}  // namespace sd_split_detail

namespace {
static constexpr auto const& sd_split
 = static_const<sd_split_detail::sd_split_fn>::value;
}  // namespace

}  // namespace hm3::geometry
