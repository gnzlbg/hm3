#ifdef ABC

#pragma once
/// \file
///
/// Intersection of a primitive with the zero-th level of a signed-distance
/// field.
#include <hm3/geometry/algorithm/sd_intersection_test.hpp>

namespace hm3::geometry {

namespace sd_intersection_detail {

struct sd_intersection_fn {
  template <typename T, typename U>
  static constexpr auto sd_intersection_impl(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(sd_intersection(std::forward<U>(u),
                                                        std::forward<T>(t)));

  template <typename T, typename U>
  static constexpr auto sd_intersection_impl(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(sd_intersection(std::forward<T>(t),
                                                        std::forward<U>(u)));

  template <typename T, typename U>
  static constexpr auto test(T&& t, U&& u) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   sd_intersection_test(std::forward<T>(t), std::forward<U>(u)));

  template <typename Rng>
  static constexpr auto test(Rng&& rng) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   sd_intersection_test(std::forward<Rng>(rng)));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(sd_intersection_impl(std::forward<T>(t),
                                                             std::forward<U>(u),
                                                             0));
};

}  // namespace sd_intersection_detail

namespace {
static constexpr auto const& sd_intersection
 = static_const<sd_intersection_detail::sd_intersection_fn>::value;
}  // namespace

}  // namespace hm3::geometry
#endif
