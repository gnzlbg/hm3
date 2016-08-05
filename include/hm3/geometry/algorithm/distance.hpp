#pragma once
/// \file
///
/// Distance computation.
#include <hm3/geometry/algorithm/centroid.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace distance_detail {

struct distance_fn {
  /// Distance from centroid to centroid
  template <typename T, typename U>
  static constexpr num_t centroid(T&& t, U&& u) noexcept {
    auto const& t_xc = hm3::geometry::centroid(std::forward<T>(t));
    auto const& u_xc = hm3::geometry::centroid(std::forward<U>(u));
    return (t_xc() - u_xc()).norm();
  }

  template <typename T, typename U>
  static constexpr auto minimum_distance_impl(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(minimum_distance(std::forward<U>(u),
                                                         std::forward<T>(t)));

  template <typename T, typename U>
  static constexpr auto minimum_distance_impl(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(minimum_distance(std::forward<T>(t),
                                                         std::forward<U>(u)));

  template <typename T, typename U>
  static constexpr auto minimum(T&& t, U&& u)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    minimum_distance_impl(std::forward<T>(t), std::forward<U>(u), 0));
};

}  // namespace detail

namespace {
static constexpr auto const& distance
 = static_const<distance_detail::distance_fn>::value;
}

}  // namespace hm3::geometry
