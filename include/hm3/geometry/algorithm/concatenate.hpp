#pragma once
/// \file
///
/// Concatenates two primitives that only overlap at their boundary.
///
/// If the primitives are oriented, concatenation preserves orientation.
#include <hm3/geometry/algorithm/concatenate/polyline_polyline.hpp>
#include <hm3/geometry/algorithm/concatenate/segment_polyline.hpp>
#include <hm3/geometry/algorithm/concatenate/segment_segment.hpp>
#include <hm3/geometry/algorithm/direction.hpp>

namespace hm3::geometry {

namespace concatenate_detail {

struct concatenate_fn {
  template <typename T, typename U,
            CONCEPT_REQUIRES_(MutablePolyline<T>{} and MutablePolyline<U>{})>
  static constexpr auto concatenate_impl(T&& t, U&& u, num_t abs_tol,
                                         num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_polyline_polyline(
    std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol));

  template <typename T, typename U,
            CONCEPT_REQUIRES_(Segment<T>{} and MutablePolyline<U>{})>
  static constexpr auto concatenate_impl(T&& t, U&& u, num_t abs_tol,
                                         num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_segment_polyline(
    std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol));

  template <typename T, CONCEPT_REQUIRES_(Segment<T>{})>
  static constexpr auto concatenate_impl(T const& t, T const& u, num_t abs_tol,
                                         num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_segment_segment(t, u,
                                                                    abs_tol,
                                                                    rel_tol));

  template <typename T, typename U>
  static constexpr auto concatenate_impl_(T&& t, U&& u, num_t abs_tol,
                                          num_t rel_tol, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_impl(std::forward<T>(t),
                                                         std::forward<U>(u),
                                                         abs_tol, rel_tol));

  template <typename T, typename U>
  static constexpr auto concatenate_impl_(T&& t, U&& u, num_t abs_tol,
                                          num_t rel_tol, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_impl(std::forward<U>(u),
                                                         std::forward<T>(t),
                                                         abs_tol, rel_tol));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_impl_(std::forward<T>(t),
                                                          std::forward<U>(u),
                                                          abs_tol, rel_tol, 0));

  /// Direction independent concatenation (does not preserve orientation).
  template <typename T, typename U>
  static constexpr auto direction_independent(T&& t, U&& u, num_t abs_tol,
                                              num_t rel_tol) {
    auto r = concatenate_fn{}(t, u, abs_tol, rel_tol);
    if (r) { return r; }
    return concatenate_fn{}(t, direction.invert(u), abs_tol, rel_tol);
  }

  template <typename T, typename U>
  static constexpr auto direction_independent(T&& t, U&& u)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(direction_independent(
    std::forward<T>(t), std::forward<U>(u), default_tolerance.absolute(),
    default_tolerance.relative()));
};

}  // namespace concatenate_detail

namespace {
static constexpr auto const& concatenate = static_const<
 with_default_tolerance<concatenate_detail::concatenate_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
