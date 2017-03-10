#pragma once
/// \file
///
// Access to matrix data
#include <hm3/math/matrix/traits.hpp>

namespace hm3::dense {

namespace detail {

/// Row-major element access
template <typename R, typename C, typename MR, typename MC>
constexpr auto matrix_idx(R&& r, C&& c, MR&&, MC&& no_cols,
                          row_major_t) noexcept {
  return r * no_cols + c;
}

/// Col-major element access
template <typename R, typename C, typename MR, typename MC>
constexpr auto matrix_idx(R&& r, C&& c, MR&& no_rows, MC&&,
                          col_major_t) noexcept {
  return c * no_rows + r;
}

}  // namespace detail

}  // namespace hm3::dense
