#pragma once
/// \file access.hpp Access to matrix data
#include <hm3/utility/matrix/traits.hpp>

namespace hm3 {
namespace dense {
namespace detail {

/// Row-major element access
template <class R, class C, class MR, class MC>
constexpr auto matrix_idx(R&& r, C&& c, MR&&, MC&& no_cols, row_major_t)
 HM3_RETURNS(r* no_cols + c);

/// Col-major element access
template <class R, class C, class MR, class MC>
constexpr auto matrix_idx(R&& r, C&& c, MR&& no_rows, MC&&, col_major_t)
 HM3_RETURNS(c* no_rows + r);

}  // namespace detail
}  // namespace dense
}  // namespace hm3
