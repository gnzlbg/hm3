#pragma once
/// \file
///
/// Are two vectors parallel?
#include <hm3/geometry/primitive/vec/cross.hpp>
#include <hm3/geometry/primitive/vec/vec.hpp>

namespace hm3::geometry::vec_primitive {

/// Are the 1D vectors \p v0 and \p v1 parallel?
///
/// Always. In particular zero vectors are always parallel to any other vector
/// including other zero vectors.
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 1 and Same<UV, vec<Nd>>{})>
constexpr bool parallel(V const&, V const&, num_t = 0.) noexcept {
  return true;
}

/// Are the 2D/3D vectors \p v0 and \p v1 parallel?
///
/// Uses the perp product:
///
/// $\mathbf{u}^{inverse_T} \cdot \mathbf{v} = |v0 x v1|$
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_((Nd == 2 or Nd == 3) and Same<UV, vec<Nd>>{})>
constexpr bool parallel(V const& v0, V const& v1, num_t tol = 0.) noexcept {
  return math::approx(perp_product_norm(v0, v1), tol);
}

}  // namespace hm3::geometry::vec_primitive
