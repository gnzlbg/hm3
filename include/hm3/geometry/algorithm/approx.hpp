#pragma once
/// \file
///
/// Are geometric primitives approximately equal?
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

namespace approx_detail {

struct tolerance_fn {
  /// Default relative tolerance of distance computations in geometric queries.
  ///
  /// \note Can be globally controlled with the macro
  /// `HM3_GEOMETRY_DISTANCE_RELATIVE_TOLERANCE`.
  static constexpr num_t relative() noexcept {
    return HM3_GEOMETRY_DISTANCE_RELATIVE_TOLERANCE;
  }

  /// Default absolute tolerance of distance computations in geometric queries.
  ///
  /// \note Can be globally controlled with the macro
  /// `HM3_GEOMETRY_DISTANCE_ABSOLUTE_TOLERANCE`.
  static constexpr num_t absolute() noexcept {
    return HM3_GEOMETRY_DISTANCE_ABSOLUTE_TOLERANCE;
  }
};

}  // namespace approx_detail

namespace {
static constexpr auto const& tolerance
 = static_const<approx_detail::tolerance_fn>::value;
}

namespace approx_detail {

/// Computes if \p a and \p b are approximately equal using the absolute and
/// relative tolerances \p `abs_tol` and \p `rel_tol`, respectively.
bool approx(num_t a, num_t b, num_t abs_tol, num_t rel_tol) noexcept {
  return std::abs(a - b)
         <= std::max(abs_tol, rel_tol * std::max(std::abs(a), std::abs(b)));
}

struct approx_fn {
  template <typename T, typename U>
  static constexpr auto impl(T&& t, U&& u, num_t abs_tol, num_t rel_tol, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(approx(std::forward<U>(u),
                                               std::forward<T>(t), abs_tol,
                                               rel_tol));

  template <typename T, typename U>
  static constexpr auto impl(T&& t, U&& u, num_t abs_tol, num_t rel_tol, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(approx(std::forward<T>(t),
                                               std::forward<U>(u), abs_tol,
                                               rel_tol));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             std::forward<U>(u),
                                             tolerance.absolute(),
                                             tolerance.relative(), 0));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             std::forward<U>(u), abs_tol,
                                             rel_tol, 0));

  template <typename T, typename U>
  static constexpr auto leq(T&& t, U&& u, num_t abs_tol, num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(t < u
                                        or impl(t, u, abs_tol, rel_tol, 0));

  template <typename T, typename U>
  static constexpr auto leq(T&& t, U&& u)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(leq(t, u, tolerance.absolute(),
                                            tolerance.relative()));

  template <typename T, typename U>
  static constexpr auto geq(T&& t, U&& u, num_t abs_tol, num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(t < u
                                        or impl(t, u, abs_tol, rel_tol, 0));

  template <typename T, typename U>
  static constexpr auto geq(T&& t, U&& u)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(geq(t, u, tolerance.absolute(),
                                            tolerance.relative()));
};

}  // namespace approx_detail

namespace {
static constexpr auto const& approx
 = static_const<approx_detail::approx_fn>::value;
}

}  // namespace hm3::geometry
