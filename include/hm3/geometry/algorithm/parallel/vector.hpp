#pragma once
/// \file
///
/// Are two vectors parallel?
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/algorithm/cross.hpp>
#include <hm3/geometry/concept/vector.hpp>
#include <hm3/geometry/tolerance.hpp>

namespace hm3::geometry {

namespace parallel_vector_detail {

struct parallel_vector_fn {
  /// Are the 1D vectors \p v0 and \p v1 parallel?
  ///
  /// Always. In particular zero vectors are always parallel to any other vector
  /// including other zero vectors.
  template <typename T, typename UT = uncvref_t<T>,
            CONCEPT_REQUIRES_(Vector<UT, 1>{})>
  static constexpr auto impl(T const&, T const&, num_t, num_t, long) noexcept {
    return true;
  }

  /// Are the 2D/3D vectors \p v0 and \p v1 parallel?
  ///
  /// Uses the perp product:
  ///
  /// $\mathbf{u}^{inverse_T} \cdot \mathbf{v} = |v0 x v1|$
  template <typename T, typename UT = uncvref_t<T>,
            CONCEPT_REQUIRES_(Vector<UT, 2>{} or Vector<UT, 3>{})>
  static constexpr auto impl(T const& v0, T const& v1, num_t abs_tol,
                             num_t rel_tol, long) noexcept {
    return approx_number(perp_product.norm(v0, v1), 0., abs_tol, rel_tol);
  }

  /// Customization point using ADL.
  template <typename T, typename UT = uncvref_t<T>,
            CONCEPT_REQUIRES_(Vector<UT>{})>
  static constexpr auto impl(T const& v0, T const& v1, num_t abs_tol,
                             num_t rel_tol, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(parallel_vector(v0, v1, abs_tol,
                                                        rel_tol));

  template <typename T, CONCEPT_REQUIRES_(Vector<uncvref_t<T>>{})>
  constexpr auto operator()(T const& v0, T const& v1, num_t abs_tol,
                            num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(v0, v1, abs_tol, rel_tol, 0));
};

}  // namespace parallel_vector_detail

namespace {
constexpr auto const& parallel_vector = static_const<
 with_default_tolerance<parallel_vector_detail::parallel_vector_fn>>::value;
}

}  // namespace hm3::geometry
