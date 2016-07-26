#pragma once
/// \file
///
/// Does one primitive intersects another?
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace detail {

struct intersection_fn {
  template <typename T, typename U>
  constexpr auto empty(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(empty_intersection(std::forward<T>(t),
                                                           std::forward<U>(u)));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection(std::forward<T>(t),
                                                     std::forward<U>(u)));

  /// Intersection of a discrete primitive with a signed distance function.
  template <typename T, typename U
            /*, CONCEPT_REQUIRES_(Discrete<T>{} and SignedDistance<U>{}) */>
  static constexpr auto signed_distance(T&& t, U&& u)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    signed_distance_intersection(std::forward<T>(t), std::forward<U>(u)));
};

}  // namespace detail

namespace {
static constexpr auto const& intersection
 = static_const<detail::intersection_fn>::value;
}  // namespace

}  // namespace discrete

using discrete::intersection;

}  // namespace geometry
}  // namespace hm3
