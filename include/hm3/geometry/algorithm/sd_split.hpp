#pragma once
/// \file
///
/// Split one primitive with the zero-th level of a signed-distance field.
#include <hm3/geometry/algorithm/sd_split/polygon.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry::sd {

namespace split_detail {

struct split_fn {
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto impl(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                             trait::polygon<Ad>) {
    static_assert(Polygon<uncvref_t<T>>{});
    static_assert(SignedDistance<uncvref_t<U>, associated::point_t<T>>{});
    static_assert(ad_v<T> == ad_v<U>);
    return split_polygon(std::forward<T>(t), std::forward<U>(u), abs_tol,
                         rel_tol);
  }

  template <typename T, typename U>
  static constexpr auto split_impl(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                   fallback)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<U>(u),
                                             std::forward<T>(t), abs_tol,
                                             rel_tol, associated::v_<U>));

  template <typename T, typename U>
  static constexpr auto split_impl(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                   preferred)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             std::forward<U>(u), abs_tol,
                                             rel_tol, associated::v_<T>));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(split_impl(std::forward<T>(t),
                                                   std::forward<U>(u), abs_tol,
                                                   rel_tol, dispatch));
};

}  // namespace split_detail

namespace {
constexpr auto const& split
 = static_const<with_default_tolerance<split_detail::split_fn>>::value;
}  // namespace

}  // namespace hm3::geometry::sd
