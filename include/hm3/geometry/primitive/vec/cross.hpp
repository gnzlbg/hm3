#pragma once
/// \file
///
/// Vector cross product and perp product.
#include <hm3/geometry/primitive/vec/vec.hpp>

namespace hm3::geometry::vec_primitive {

/// Vec cross-product (3D only).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 3 and Same<UV, vec<Nd>>{})>
constexpr UV cross(V const& v0, V const& v1) noexcept {
  return UV{v0().cross(v1())};
}

/// Vec cross-product (2D only, returns a 3D vector).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 2 and Same<UV, vec<Nd>>{})>
constexpr auto cross(V const& v0, V const& v1) noexcept {
  vec<3> v0_{v0(0), v0(1), 0.};
  vec<3> v1_{v1(0), v1(1), 0.};
  return cross(v0_, v1_);
}

/// Computes the perp product of the 2D vectors \p v0 and \p v1.
///
/// Defined as:
///
/// $\mathbf{u}^{inverse_T} \cdot \mathbf{v} = u_0v_1 - u_1 v_0$
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 2 and Same<UV, vec<Nd>>{})>
constexpr num_t perp_product(V const& v0, V const& v1) noexcept {
  return v0(0) * v1(1) - v0(1) * v1(0);
}

template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 2 and Same<UV, vec<Nd>>{})>
constexpr num_t perp_product_norm(V const& v0, V const& v1) noexcept {
  return std::abs(perp_product(v0, v1));
}

/// Computes the perp product of the 3D vectors \p v0 and \p v1.
///
/// Defined as: $|v0 x v1|$
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 3 and Same<UV, vec<Nd>>{})>
constexpr num_t perp_product_norm(V const& v0, V const& v1) noexcept {
  return v0().cross(v1()).norm();
}

}  // namespace hm3::geometry::vec_primitive
