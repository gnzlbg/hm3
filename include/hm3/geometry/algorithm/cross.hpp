#pragma once
/// \file
///
/// Cross product and perp product.
#include <hm3/geometry/concept/vector.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace cross_detail {

struct cross_fn {
  /// Vec cross-product (3D only).
  template <typename T, typename UT = uncvref_t<T>,
            CONCEPT_REQUIRES_(Vector<T, 3>{})>
  static UT impl(T const& v0, T const& v1) noexcept {
    return UT{v0().cross(v1())};
  }

  /// Vec cross-product (2D only, returns a 3D vector).
  template <typename T, typename UT = uncvref_t<T>,
            typename OT = trait::rebind_t<T, 3>,
            CONCEPT_REQUIRES_(Vector<T, 2>{})>
  static UT impl(T const& v0, T const& v1) noexcept {
    OT v0_{v0(0), v0(1), 0.};
    OT v1_{v1(0), v1(1), 0.};
    return cross(v0_, v1_, trait::vector<3>{});
  }

  template <typename T, CONCEPT_REQUIRES_(Vector<T>{})>
  constexpr auto operator()(T const& v0, T const& v1) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(v0, v1));
};

}  // namespace cross_detail

namespace {
constexpr auto const& cross = static_const<cross_detail::cross_fn>::value;
}

namespace perp_product_detail {

struct perp_product_fn {
  /// Computes the perp product of the 3D vectors \p v0 and \p v1.
  ///
  /// Defined as: $|v0 x v1|$
  template <typename T, CONCEPT_REQUIRES_(Vector<T, 3>{})>
  static associated::num_type_t<T> impl(T const& v0, T const& v1) noexcept {
    return v0().cross(v1()).norm();
  }

  /// Computes the perp product of the 2D vectors \p v0 and \p v1.
  ///
  /// Defined as:
  ///
  /// $\mathbf{u}^{inverse_T} \cdot \mathbf{v} = u_0v_1 - u_1 v_0$
  template <typename T, CONCEPT_REQUIRES_(Vector<T, 2>{})>
  static associated::num_type_t<T> impl(T const& v0, T const& v1) noexcept {
    return v0(0) * v1(1) - v0(1) * v1(0);
  }

  template <typename T, CONCEPT_REQUIRES_(Vector<T>{})>
  constexpr auto operator()(T const& v0, T const& v1) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(v0, v1));

  template <typename T, CONCEPT_REQUIRES_(Vector<T>{})>
  static constexpr associated::num_type_t<T> norm(T const& v0,
                                                  T const& v1) noexcept {
    return std::abs(perp_product_fn{}(v0, v1));
  }
};

}  // namespace perp_product_detail

namespace {
constexpr auto const& perp_product
 = static_const<perp_product_detail::perp_product_fn>::value;
}

}  // namespace hm3::geometry
