#pragma once
/// \file eigen.hpp Includes the eigen library
#include <hm3/types.hpp>
#include <hm3/utility/returns.hpp>

#define EIGEN_DEFAULT_DENSE_INDEX_TYPE ::hm3::int_t
#include <Eigen/Dense>
#include <Eigen/StdVector>

namespace hm3 {
namespace traits {

/// Detects if T is an Eigen::Matrix type
template <class T> struct is_eigen_matrix : std::false_type {};
template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
struct is_eigen_matrix<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows,
                                     _MaxCols>> : std::true_type {};

}  // namespace traits

template <class EigenType>
using aligned_eigen_map = Eigen::Map<EigenType, Eigen::Aligned>;

template <class T, int noRows, int noCols, int Order, int maxRows = noRows,
          int maxCols                                             = noCols>
using aligned_eigen_type
 = Eigen::Matrix<T, noRows, noCols, Order | Eigen::AutoAlign, maxRows, maxCols>;

}  // namespace hm3

/// Eigen extensions:
namespace Eigen {

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto begin(Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 HM3_RETURNS(v.data());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto end(Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 HM3_RETURNS(v.data() + v.size());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto begin(const Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 HM3_RETURNS(v.data());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto end(const Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 HM3_RETURNS(v.data() + v.size());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto cbegin(
 const Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 HM3_RETURNS(v.data());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto cend(const Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 HM3_RETURNS(v.data() + v.size());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto range_begin(Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 HM3_RETURNS(v.data());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto range_end(Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 HM3_RETURNS(v.data() + v.size());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto range_begin(
 const Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 HM3_RETURNS(v.data());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto range_end(
 const Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 HM3_RETURNS(v.data() + v.size());

}  // namespace Eigen
