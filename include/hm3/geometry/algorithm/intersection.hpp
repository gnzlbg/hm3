#pragma once
/// \file
///
/// Does one primitive intersects another?
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace intersection_detail {

struct intersection_fn {
  template <typename T, typename U>
  static constexpr auto intersection_test_impl(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_test(std::forward<U>(u),
                                                          std::forward<T>(t)));

  template <typename T, typename U>
  static constexpr auto intersection_test_impl(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_test(std::forward<T>(t),
                                                          std::forward<U>(u)));

  template <typename T, typename U>
  static constexpr auto intersection_impl(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection(std::forward<U>(u),
                                                     std::forward<T>(t)));

  template <typename T, typename U>
  static constexpr auto intersection_impl(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection(std::forward<T>(t),
                                                     std::forward<U>(u)));

  template <typename T, typename U>
  static constexpr auto test(T&& t, U&& u) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   intersection_test_impl(std::forward<T>(t), std::forward<U>(u), 0));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_impl(std::forward<T>(t),
                                                          std::forward<U>(u),
                                                          0));
};

}  // namespace intersection_detail

namespace {
static constexpr auto const& intersection
 = static_const<intersection_detail::intersection_fn>::value;
}  // namespace

}  // namespace hm3::geometry
