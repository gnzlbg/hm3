#pragma once
/// \file
///
/// Includes the Eigen library
#include <hm3/ext/vector.hpp>
#include <hm3/types.hpp>
/// Select the default dense index type:
#define EIGEN_DEFAULT_DENSE_INDEX_TYPE ::hm3::int_t

// Eigen3 does still use dynamic exception specifications which are deprecated
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"

#include <Eigen/Dense>
#include <Eigen/StdVector>

#pragma clang diagnostic pop

namespace hm3 {

template <typename EigenType>
using aligned_eigen_map = Eigen::Map<EigenType, Eigen::Aligned>;

template <typename T, int NoRows, int NoCols, int Order, int MaxRows = NoRows,
          int MaxCols = NoCols>
using aligned_eigen_type
 = Eigen::Matrix<T, NoRows, NoCols, Order | Eigen::AutoAlign, MaxRows, MaxCols>;

template <typename T>
using vector = std_vector<T, Eigen::aligned_allocator<T>>;

}  // namespace hm3

/// Eigen extensions:
namespace Eigen {  // NOLINT

template <typename Scalar, int Rows, int Cols, int Options, int MaxRows,
          int MaxCols>
auto begin(Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& v) noexcept {
  return v.data();
}

template <typename Scalar, int Rows, int Cols, int Options, int MaxRows,
          int MaxCols>
auto end(Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& v) noexcept {
  return v.data()
         + v.size();  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

template <typename Scalar, int Rows, int Cols, int Options, int MaxRows,
          int MaxCols>
auto begin(
 const Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& v) noexcept {
  return v.data();
}

template <typename Scalar, int Rows, int Cols, int Options, int MaxRows,
          int MaxCols>
auto end(
 const Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& v) noexcept {
  return v.data()
         + v.size();  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

template <typename Scalar, int Rows, int Cols, int Options, int MaxRows,
          int MaxCols>
auto cbegin(
 const Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& v) noexcept {
  return v.data();
}

template <typename Scalar, int Rows, int Cols, int Options, int MaxRows,
          int MaxCols>
auto cend(
 const Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& v) noexcept {
  return v.data()
         + v.size();  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

template <typename Scalar, int Rows, int Cols, int Options, int MaxRows,
          int MaxCols>
auto range_begin(
 Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& v) noexcept {
  return v.data();
}

template <typename Scalar, int Rows, int Cols, int Options, int MaxRows,
          int MaxCols>
auto range_end(
 Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& v) noexcept {
  return v.data()
         + v.size();  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

template <typename Scalar, int Rows, int Cols, int Options, int MaxRows,
          int MaxCols>
auto range_begin(
 const Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& v) noexcept {
  return v.data();
}

template <typename Scalar, int Rows, int Cols, int Options, int MaxRows,
          int MaxCols>
auto range_end(
 const Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>& v) noexcept {
  return v.data()
         + v.size();  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

}  // namespace Eigen
