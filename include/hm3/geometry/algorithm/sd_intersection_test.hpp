#pragma once
/// \file
///
/// Does a primitive intersect the zero-th level of a signed-distance field?
#include <hm3/geometry/algorithm/sd_relative_position.hpp>
#include <hm3/geometry/concept/signed_distance.hpp>

namespace hm3::geometry::sd {

namespace intersection_test_detail {

struct intersection_test_fn {
  /// Is the primitive \p p intersected by the signed-distance field \p sdf?
  template <
   typename P, typename SD,
   CONCEPT_REQUIRES_(SignedDistance<uncvref_t<SD>, associated::point_t<P>>{})>
  static constexpr auto impl(P&& p, SD&& sdf, num_t abs_tol,
                             num_t rel_tol) noexcept {
    return relative_position(
            vertices(p) | view::transform([&sdf](auto&& x) { return sdf(x); }),
            abs_tol, rel_tol)
             == intersected
            ? true
            : false;
  }

  template <typename T, typename U>
  static constexpr auto impl_select(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                    long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<U>(u),
                                             std::forward<T>(t), abs_tol,
                                             rel_tol));

  template <typename T, typename U>
  static constexpr auto impl_select(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                    int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             std::forward<U>(u), abs_tol,
                                             rel_tol));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl_select(std::forward<T>(t),
                                                    std::forward<U>(u), abs_tol,
                                                    rel_tol, 0));

  template <typename Rng>
  constexpr auto operator()(Rng&& rng, num_t abs_tol, num_t rel_tol) const
   noexcept {
    static_assert(Range<Rng>{});
    using VT = ranges::range_value_t<Rng>;
    static_assert(Same<VT, num_t>{} or Same<VT, math::signum_t>{});
    return relative_position(rng, abs_tol, rel_tol) == intersected ? true
                                                                   : false;
  }
};

}  // namespace intersection_test_detail

namespace {
static constexpr auto const& intersection_test = static_const<
 with_default_tolerance<intersection_test_detail::intersection_test_fn>>::value;
}  // namespace

}  // namespace hm3::geometry::sd
