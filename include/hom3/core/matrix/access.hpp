#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Utilities for accessing matrix data
////////////////////////////////////////////////////////////////////////////////
#include <hom3/core/at.hpp>
#include <hom3/core/arithmetic.hpp>
#include <hom3/core/assert.hpp>
#include <hom3/core/types.hpp>
#include <hom3/core/matrix/traits.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace hom3 {

namespace dense {

namespace detail {

template <class RowIdx, class ColIdx, class RowSize, class ColSize, class Size>
inline void assert_matrix_indices(const RowIdx i, const ColIdx j,
                                  const RowSize noRows, const ColSize noCols,
                                  const Size size, const String at) {
  ASSERT_AT(primitive_cast(i) < noRows,
            "row index = " + std::to_string(primitive_cast(i))
            + " is out of bounds: [0, " + std::to_string(noRows) + ")."
            + " Col index = " + std::to_string(primitive_cast(j))
            + " has bounds: [0, " + std::to_string(noCols) + ").",
            at);
  ASSERT_AT(primitive_cast(j) < noCols,
            "col index = " + std::to_string(primitive_cast(j))
            + " is out of bounds: [0, " + std::to_string(noCols) + ")."
            + " Row index = " + std::to_string(primitive_cast(i))
            + " has bounds: [0, " + std::to_string(noRows) + ").",
            at);
  ASSERT_AT(primitive_cast(i) * primitive_cast(j) < size,
            "matrix element (" + std::to_string(primitive_cast(i)) + ", "
            + std::to_string(primitive_cast(j))
            + ") is out of memory bounds: [0," + std::to_string(size) + ").",
            at);
}

template <class RowIdx, class ColIdx, class IntegralR, class IntegralC,
          class IntegralS>
inline auto index(row_major_t, const RowIdx row, const ColIdx col,
                  const IntegralR noRows, const IntegralC noCols,
                  const IntegralS size, const String at) {
  assert_matrix_indices(row, col, noRows, noCols, size, at);
  return primitive_cast(row) * noCols + primitive_cast(col);
}

template <class RowIdx, class ColIdx, class IntegralR, class IntegralC,
          class IntegralS>
inline auto index(col_major_t, const RowIdx row, const ColIdx col,
                  const IntegralR noRows, const IntegralC noCols,
                  const IntegralS size, const String at) {
  assert_matrix_indices(row, col, noRows, noCols, size, at);
  return primitive_cast(col) * noRows + primitive_cast(row);
}

/// \brief Assert vector index:
template <class Index, class Size>
inline void assert_vector_index(const Index index, const Size size,
                                const String at) {
  ASSERT_AT(primitive_cast(index) < size,
            "vector index = " + std::to_string(primitive_cast(index))
            + " is out of bounds: [0, " + std::to_string(size) + ").",
            at);
}

template <class Index, class Integral>
inline auto index(const Index row, const Integral noRows, const String at) {
  assert_vector_index(row, noRows, at);
  return primitive_cast(row);
}

}  // namespace detail

using detail::index;
using detail::assert_matrix_indices;
}  // namespace dense
}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
