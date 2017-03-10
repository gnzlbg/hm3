#pragma once
/// \file
///
/// Are two points or vectors approximately equal
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/concept/vector.hpp>
#include <hm3/geometry/tolerance.hpp>

namespace hm3::geometry {

namespace approx_point_detail {

template <typename T, typename NT>
constexpr bool approx_point_or_vector(T const& a, T const& b, NT abs_tol,
                                      NT rel_tol) noexcept {
  constexpr auto ad = ad_v<T>;
  for (dim_t d = 0; d < ad; ++d) {
    if (not approx_number(a(d), b(d), abs_tol, rel_tol)) { return false; }
  }
  return true;
}

template <typename T, typename NT>
constexpr bool geq_point_or_vector(T const& a, T const& b, NT abs_tol,
                                   NT rel_tol) noexcept {
  constexpr auto ad = ad_v<T>;
  for (dim_t d = 0; d < ad; ++d) {
    if (not(a(d) >= b(d) or approx_number(a(d), b(d), abs_tol, rel_tol))) {
      return false;
    }
  }
  return true;
}

template <typename T, typename NT>
constexpr bool leq_point_or_vector(T const& a, T const& b, NT abs_tol,
                                   NT rel_tol) noexcept {
  constexpr auto ad = ad_v<T>;
  for (dim_t d = 0; d < ad; ++d) {
    if (not(a(d) <= b(d) or approx_number(a(d), b(d), abs_tol, rel_tol))) {
      return false;
    }
  }
  return true;
}

struct approx_point_fn {
  template <typename T, typename NT>
  constexpr bool operator()(T const& a, T const& b, NT abs_tol,
                            NT rel_tol) const noexcept {
    static_assert(Point<T>{});
    return approx_point_or_vector(a, b, abs_tol, rel_tol);
  }

  template <typename T, typename NT>
  static constexpr bool geq(T const& a, T const& b, NT abs_tol,
                            NT rel_tol) noexcept {
    static_assert(Point<T>{});
    return geq_point_or_vector(a, b, abs_tol, rel_tol);
  }

  template <typename T, typename NT>
  static constexpr bool leq(T const& a, T const& b, NT abs_tol,
                            NT rel_tol) noexcept {
    static_assert(Point<T>{});
    return leq_point_or_vector(a, b, abs_tol, rel_tol);
  }
};

struct approx_vector_fn {
  template <typename T, typename NT>
  constexpr bool operator()(T const& a, T const& b, NT abs_tol,
                            NT rel_tol) const noexcept {
    static_assert(Vector<T>{});
    return approx_point_or_vector(a, b, abs_tol, rel_tol);
  }

  template <typename T, typename NT>
  static constexpr bool geq(T const& a, T const& b, NT abs_tol,
                            NT rel_tol) noexcept {
    static_assert(Vector<T>{});
    return geq_point_or_vector(a, b, abs_tol, rel_tol);
  }

  template <typename T, typename NT>
  static constexpr bool leq(T const& a, T const& b, NT abs_tol,
                            NT rel_tol) noexcept {
    static_assert(Vector<T>{});
    return leq_point_or_vector(a, b, abs_tol, rel_tol);
  }
};

}  // namespace approx_point_detail

namespace {
constexpr auto const& approx_point = static_const<
 with_default_tolerance<approx_point_detail::approx_point_fn>>::value;

constexpr auto const& approx_vector = static_const<
 with_default_tolerance<approx_point_detail::approx_vector_fn>>::value;

}  // namespace

}  // namespace hm3::geometry
