#pragma once
/// \file
///
/// Parallel
#include <hm3/geometry/algorithm/parallel/line.hpp>
#include <hm3/geometry/algorithm/parallel/plane.hpp>
#include <hm3/geometry/algorithm/parallel/vector.hpp>
#include <hm3/geometry/tolerance.hpp>

namespace hm3::geometry {

namespace parallel_detail {

struct parallel_fn {
  /// Vector dispatch
  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto impl(T const& v0, T const& v1, num_t abs_tol,
                             num_t rel_tol, trait::vector<Ad>) noexcept {
    static_assert(Vector<T>{});
    return parallel_vector(v0, v1, abs_tol, rel_tol);
  }

  /// Line dispatch
  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr bool impl(T const& a, T const& b, num_t abs_tol,
                             num_t rel_tol, trait::line<Ad>) noexcept {
    static_assert(Line<T>{});
    return parallel_line(a, b, abs_tol, rel_tol);
  }

  /// Ray dispatch
  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr bool impl(T const& a, T const& b, num_t abs_tol,
                             num_t rel_tol, trait::ray<Ad>) noexcept {
    static_assert(Ray<T>{});
    return parallel_line(a, b, abs_tol, rel_tol);
  }

  /// Plane dispatch
  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr bool impl(T const& a, T const& b, num_t abs_tol,
                             num_t rel_tol, trait::plane<Ad>) noexcept {
    static_assert(Plane<T>{});
    return parallel_plane(a, b, abs_tol, rel_tol);
  }

  template <typename T>
  constexpr auto operator()(T&& v0, T&& v1, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(v0),
                                             std::forward<T>(v1), abs_tol,
                                             rel_tol, associated::v_<T>));
};

}  // namespace parallel_detail

namespace {
constexpr auto const& parallel
 = static_const<with_default_tolerance<parallel_detail::parallel_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
