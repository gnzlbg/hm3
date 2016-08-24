#pragma once
/// \file
///
/// Dense matrix traits
#include <hm3/types.hpp>
#include <hm3/utility/matrix/eigen.hpp>

namespace hm3 {
namespace dense {

/// Dynamic dimension
static const constexpr int_t dynamic = -1;

/// Row major Order
struct row_major_t {};
static const constexpr row_major_t row_major{};

/// Col major Order
struct col_major_t {};
static const constexpr row_major_t col_major{};

/// Dynamic storage type
struct dynamic_storage {};

/// Stack storage type
struct stack_storage {};

/// Bit storage
struct bit {};

template <typename Order>
struct to_eigen_order {};

template <>
struct to_eigen_order<row_major_t> {
  static constexpr int_t value = Eigen::RowMajor;
};

template <>
struct to_eigen_order<col_major_t> {
  static constexpr int_t value = Eigen::ColMajor;
};

/// Select storage type:
//  if MaxRows != dynamic && MaxCols != dynamic
//  => stack storage
template <int_t MaxRows, int_t MaxCols>
struct storage_type {
  using type = stack_storage;
};

/// Select storage type:
//  if MaxRows == dynamic
//  => dynamic storage
template <int_t MaxCols>
struct storage_type<dynamic, MaxCols> {
  using type = dynamic_storage;
};

/// Select storage type:
//  if MaxCols == dynamic
//  => dynamic storage
template <int_t MaxRows>
struct storage_type<MaxRows, dynamic> {
  using type = dynamic_storage;
};

/// Select storage type:
//  if MaxRows == dynamic && MaxCols == dynamic
//  => dynamic storage
template <>
struct storage_type<dynamic, dynamic> {
  using type = dynamic_storage;
};

template <int_t MaxRows, int_t MaxCols>
using storage_type_t = typename storage_type<MaxRows, MaxCols>::type;

}  // namespace dense
}  // namespace hm3
