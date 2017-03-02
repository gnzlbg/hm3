#pragma once
/// \file
///
/// Are two lines parallel?
#include <hm3/geometry/algorithm/direction/line.hpp>
#include <hm3/geometry/algorithm/parallel/vector.hpp>
#include <hm3/geometry/concept/line.hpp>
#include <hm3/geometry/concept/ray.hpp>
#include <hm3/geometry/tolerance.hpp>

namespace hm3::geometry {

namespace parallel_line_detail {

struct parallel_line_fn {
  /// Are the 1D lines \p v0 and \p v1 parallel? (always)
  template <typename T, typename UT = uncvref_t<T>,
            CONCEPT_REQUIRES_(LineOrRay<UT, 1>{})>
  static constexpr auto impl(T const&, T const&, num_t, num_t, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(true);

  /// Are the 2D/3D lines \p a and \p b parallel?
  template <typename T, typename UT = uncvref_t<T>,
            CONCEPT_REQUIRES_(LineOrRay<UT, 2>{} or LineOrRay<UT, 3>{})>
  static constexpr auto impl(T const& a, T const& b, num_t abs_tol,
                             num_t rel_tol, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(parallel_vector(direction_line(a),
                                                        direction_line(b),
                                                        abs_tol, rel_tol));

  /// Customization point using ADL.
  template <typename T, typename UT = uncvref_t<T>,
            CONCEPT_REQUIRES_(LineOrRay<UT>{})>
  static constexpr auto impl(T const& a, T const& b, num_t abs_tol,
                             num_t rel_tol, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(parallel_line(a, b, abs_tol, rel_tol));

  template <typename T, CONCEPT_REQUIRES_(LineOrRay<T>{})>
  constexpr auto operator()(T const& a, T const& b, num_t abs_tol,
                            num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(a, b, abs_tol, rel_tol, 0));
};

}  // namespace parallel_line_detail

namespace {
static constexpr auto const& parallel_line = static_const<
 with_default_tolerance<parallel_line_detail::parallel_line_fn>>::value;
}

}  // namespace hm3::geometry
