#pragma once
/// \file
///
/// Set union.
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace set_union_detail {

struct set_union_fn {
  template <typename T, typename U>
  static constexpr auto set_union_impl(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(set_union(std::forward<U>(u),
                                                  std::forward<T>(t)));

  template <typename T, typename U>
  static constexpr auto set_union_impl(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(set_union(std::forward<T>(t),
                                                  std::forward<U>(u)));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(set_union_impl(std::forward<T>(t),
                                                       std::forward<U>(u), 0));
};

}  // namespace set_union_detail

namespace {
static constexpr auto const& set_union
 = static_const<set_union_detail::set_union_fn>::value;
}  // namespace

}  // namespace hm3::geometry
