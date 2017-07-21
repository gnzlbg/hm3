#pragma once
/// \file
///
/// Dense matrix traits
#include <hm3/ext/eigen.hpp>
#include <hm3/types.hpp>

namespace hm3::dense {

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
  static constexpr inline int_t value = Eigen::RowMajor;
};

template <>
struct to_eigen_order<col_major_t> {
  static constexpr inline int_t value = Eigen::ColMajor;
};

/// Select storage type:
//  if MaxRows != dynamic && MaxCols != dynamic && heap_only == false
//  => stack storage
template <int_t MaxRows, int_t MaxCols, bool heap_only>
struct storage_type {
  using type = stack_storage;
};

template <int_t MaxRows, int_t MaxCols>
struct storage_type<MaxRows, MaxCols, true> {
  using type = dynamic_storage;
};

/// Select storage type:
//  if MaxRows == dynamic
//  => dynamic storage
template <int_t MaxCols, bool heap_only>
struct storage_type<dynamic, MaxCols, heap_only> {
  using type = dynamic_storage;
};

/// Select storage type:
//  if MaxCols == dynamic
//  => dynamic storage
template <int_t MaxRows, bool heap_only>
struct storage_type<MaxRows, dynamic, heap_only> {
  using type = dynamic_storage;
};

/// Select storage type:
//  if MaxRows == dynamic && MaxCols == dynamic
//  => dynamic storage
template <bool heap_only>
struct storage_type<dynamic, dynamic, heap_only> {
  using type = dynamic_storage;
};

template <int_t MaxRows, int_t MaxCols, bool heap_only>
using storage_type_t = typename storage_type<MaxRows, MaxCols, heap_only>::type;

}  // namespace hm3::dense
