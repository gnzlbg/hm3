#pragma once
/// \file
///
/// Distance computation.
#include <hm3/geometry/algorithm/centroid.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

// Relative tolerance of the point segment intersection
#ifndef HM3_GEOMETRY_DISTANCE_RELATIVE_TOLERANCE
#define HM3_GEOMETRY_DISTANCE_RELATIVE_TOLERANCE 1e-16
#endif

// Absolute tolerance of the point segment intersection
#ifndef HM3_GEOMETRY_DISTANCE_ABSOLUTE_TOLERANCE
#define HM3_GEOMETRY_DISTANCE_ABSOLUTE_TOLERANCE 1e-14
#endif

namespace hm3::geometry {

namespace distance_detail {

struct distance_fn {
  /// Distance between the centroids of \p t and \p u.
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
  /// Minimum distance between two geometry primitivies \p t and\p u.
  template <typename T, typename U>
  static constexpr auto minimum(T&& t, U&& u)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    minimum_distance_impl(std::forward<T>(t), std::forward<U>(u), 0));

  /// Default relative tolerance of distance computations.
  ///
  /// \note Can be globally controlled with the macro
  /// `HM3_GEOMETRY_DISTANCE_RELATIVE_TOLERANCE`.
  static constexpr auto rel_tol() noexcept {
    return HM3_GEOMETRY_DISTANCE_RELATIVE_TOLERANCE;
  }
  /// Default absolute tolerance of distance computations.
  ///
  /// \note Can be globally controlled with the macro
  /// `HM3_GEOMETRY_DISTANCE_ABSOLUTE_TOLERANCE`.
  static constexpr auto abs_tol() noexcept {
    return HM3_GEOMETRY_DISTANCE_ABSOLUTE_TOLERANCE;
  }

  /// Computes if \p a and \p b are approximately equal using the absolute and
  /// relative tolerances \p `abs_tol` and \p `rel_tol`, respectively.
  static auto approx(num_t a, num_t b, num_t abs_tol, num_t rel_tol) noexcept {
    return std::abs(a - b)
           <= std::max(abs_tol, rel_tol * std::max(std::abs(a), std::abs(b)));
  }
  /// Computes if \p a and \p b are approximately equal using default
  /// tolerances.
  static auto approx(num_t a, num_t b) noexcept {
    return approx(a, b, abs_tol(), rel_tol());
  }

  /// Computes if the points \p a and \p b are approximately equal using the
  /// absolute and relative tolerances \p `abs_tol` and \p `rel_tol`,
  /// respectively.
  template <dim_t Nd>
  static auto approx(point<Nd> a, point<Nd> b, num_t abs_tol, num_t rel_tol) {
    for (dim_t d = 0; d < Nd; ++d) {
      if (!approx(a(d), b(d), abs_tol, rel_tol)) { return false; }
    }
    return true;
  }

  /// Computes if the points \p a and \p b are approximately equal using default
  /// tolerances.
  template <dim_t Nd>
  static auto approx(point<Nd> a, point<Nd> b) {
    return approx(a, b, abs_tol(), rel_tol());
  }
};

}  // namespace detail

namespace {
static constexpr auto const& distance
 = static_const<distance_detail::distance_fn>::value;
}

}  // namespace hm3::geometry
