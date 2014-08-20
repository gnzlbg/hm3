#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Includes the eigen library
////////////////////////////////////////////////////////////////////////////////
#include <hom3/core/types.hpp>
#include <boost/range.hpp>
#include <boost/range/config.hpp>
#include <boost/range/iterator.hpp>
#include <boost/range/mutable_iterator.hpp>
#include <boost/range/difference_type.hpp>

////////////////////////////////////////////////////////////////////////////////
#define EIGEN_DEFAULT_DENSE_INDEX_TYPE ::hom3::Int
#include <Eigen/Dense>
#include <Eigen/StdVector>

namespace hom3 {
namespace traits {

/// Detects if T is an Eigen::Matrix type
template <class T> struct is_eigen_matrix { static const bool value = false; };
template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
struct is_eigen_matrix<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows,
                                     _MaxCols>> {
  static const bool value = true;
};

}  // namespace traits
}  // namespace hom3

/// Eigen extensions:
namespace Eigen {

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto begin(Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 RETURNS(v.data());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto end(Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 RETURNS(v.data() + v.size());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto begin(const Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 RETURNS(v.data());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto end(const Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 RETURNS(v.data() + v.size());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto range_begin(Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 RETURNS(v.data());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto range_end(Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 RETURNS(v.data() + v.size());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto range_begin(
 const Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 RETURNS(v.data());

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
auto range_end(
 const Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& v)
 RETURNS(v.data() + v.size());

}  // namespace Eigen

/// Boost range extension for Eigen types
namespace boost {

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
inline BOOST_DEDUCED_TYPENAME
 range_difference<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows,
                                _MaxCols>>::type
 range_calculate_size(const Eigen::Matrix<_Scalar, _Rows, _Cols, _Options,
                                          _MaxRows, _MaxCols>& rng) {
  return rng.size();
}

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
struct range_mutable_iterator<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options,
                                            _MaxRows, _MaxCols>> {
  using type = _Scalar*;
};

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
struct range_const_iterator<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options,
                                          _MaxRows, _MaxCols>> {
  using type = const _Scalar*;
};

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols, int BlockRows, int BlockCols, bool InnerPanel>
struct range_const_iterator<Eigen::Block<Eigen::Matrix<_Scalar, _Rows, _Cols,
                                                       _Options, _MaxRows,
                                                       _MaxCols>,
                                         BlockRows, BlockCols, InnerPanel>> {
  using type = const _Scalar*;
};

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols, int BlockRows, int BlockCols, bool InnerPanel>
struct range_const_iterator<Eigen::Block<const Eigen::
                                          Matrix<_Scalar, _Rows, _Cols,
                                                 _Options, _MaxRows, _MaxCols>,
                                         BlockRows, BlockCols, InnerPanel>> {
  using type = const _Scalar*;
};

template <class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
          int _MaxCols>
struct range_iterator<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows,
                                    _MaxCols>> {
  using type = _Scalar*;
};

}  // namespace boost
