#pragma once
/// \file
///
/// Merge two geometric primitives into one.
#include <hm3/geometry/algorithm/merge/segments.hpp>
#include <hm3/geometry/concept/segment.hpp>
#include <hm3/geometry/tolerance.hpp>

namespace hm3::geometry {

namespace merge_detail {

struct merge_fn {
  /// Merge the segments \p s0 and \p s1.
  template <typename S0, typename S1, dim_t Ad = ad_v<S0>>
  static constexpr auto merge_impl(S0 const& s0, S1 const& s1, num_t abs_tol,
                                   num_t rel_tol, trait::segment<Ad>,
                                   trait::segment<Ad>) noexcept {
    static_assert(Segment<S0>{});
    static_assert(Segment<S1>{});
    static_assert(ad_v<S0> == ad_v<S1>);
    return merge_segments(s0, s1, abs_tol, rel_tol);
  }

  template <typename T, typename U>
  constexpr auto merge_dispatch(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                preferred) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(merge_impl(std::forward<T>(t),
                                                   std::forward<U>(u), abs_tol,
                                                   rel_tol, associated::v_<T>,
                                                   associated::v_<U>));

  template <typename T, typename U>
  constexpr auto merge_dispatch(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                fallback) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(merge_impl(std::forward<U>(u),
                                                   std::forward<T>(t), abs_tol,
                                                   rel_tol, associated::v_<U>,
                                                   associated::v_<T>));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(merge_dispatch(
    std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol, dispatch));
};

}  // namespace merge_detail

namespace {
constexpr auto const& merge
 = static_const<with_default_tolerance<merge_detail::merge_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
