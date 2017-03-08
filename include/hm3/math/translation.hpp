#pragma once
/// \file
///
/// Translation matrices.
#include <hm3/ext/eigen.hpp>
#include <hm3/math/constants.hpp>
#include <hm3/types.hpp>

namespace hm3::math {

/// Translation matrix by \v.
template <typename V, dim_t Ad>
Eigen::Translation<num_t, Ad> translation(V v, dim_vt<Ad>) {
  return Eigen::Translation<num_t, Ad>(v);
}

/// Inverse translation matrix by \v.
template <typename V, dim_t Ad>
Eigen::Translation<num_t, Ad> inverse_translation(V v, dim_vt<Ad>) {
  return translation(v, dim_v<Ad>).inverse();
}

}  // namespace hm3::math
