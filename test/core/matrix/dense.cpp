/// \file \brief Tests for hyperoctree::multi_grid_hyperoctree
/// Includes:
#include <hom3/core/matrix/dense.hpp>
#include <hom3/core/ranges.hpp>
#include <hom3/core/test.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/mpl.hpp>
////////////////////////////////////////////////////////////////////////////////
using namespace hom3;

////////////////////////////////////////////////////////////////////////////////
/// Explicit instantiations:
// dense::detail::dense_matrix_base<T, dense::detail::default_vector,
//                                  noRows, noCols,
//                                  RowIdx, ColIdx,
//                                  order,
//                                  maxRows, maxCols>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Explicit instantiations for vector:
/// Vector, dynamic storage, dynamic size, col-major order:
/// static noCols == 1, rows == dynamic
template struct dense::detail::
 dense_matrix_base<Num, dense::detail::default_vector, dense::dynamic, 1, Int,
                   Int, dense::col_major_t, dense::dynamic, 1>;
template struct dense::detail::
 dense_matrix_base<Num, dense::detail::default_vector, dense::dynamic, 1,
                   arithmetic<unsigned, void>, arithmetic<unsigned, void>,
                   dense::col_major_t, dense::dynamic, 1>;

/// Vector, static storage, static size, col-major order:
/// noCols == 1 (static), noRows (static)
template struct dense::detail::dense_matrix_base<Num,
                                                 dense::detail::default_vector,
                                                 10, 1, Int, Int,
                                                 dense::col_major_t, 10, 1>;
template struct dense::detail::
 dense_matrix_base<Num, dense::detail::default_vector, 10, 1,
                   arithmetic<unsigned, void>, arithmetic<unsigned, void>,
                   dense::col_major_t, 10, 1>;

/// Vector, static storage, dynamic size, col-major order:
/// noRows == dynamic, noCols == 1 (static), maxRows (static)
template struct dense::detail::dense_matrix_base<Num,
                                                 dense::detail::default_vector,
                                                 dense::dynamic, 1, Int, Int,
                                                 dense::col_major_t, 10, 1>;
template struct dense::detail::
 dense_matrix_base<Num, dense::detail::default_vector, dense::dynamic, 1,
                   arithmetic<unsigned, void>, arithmetic<unsigned, void>,
                   dense::col_major_t, 10, 1>;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Explicit instantiations for matrix:

/// Fully dynamic col/row order matrix
template struct dense::detail::
 dense_matrix_base<Num, dense::detail::default_vector, dense::dynamic,
                   dense::dynamic, Int, Int, dense::col_major_t, dense::dynamic,
                   dense::dynamic>;
template struct dense::detail::
 dense_matrix_base<Num, dense::detail::default_vector, dense::dynamic,
                   dense::dynamic, arithmetic<unsigned, void>,
                   arithmetic<unsigned, void>, dense::row_major_t,
                   dense::dynamic, dense::dynamic>;

/// Fully static col/row matrix
template struct dense::detail::dense_matrix_base<Num,
                                                 dense::detail::default_vector,
                                                 10, 20, Int, Int,
                                                 dense::col_major_t, 10, 20>;
template struct dense::detail::
 dense_matrix_base<Num, dense::detail::default_vector, 10, 20,
                   arithmetic<unsigned, void>, arithmetic<unsigned, void>,
                   dense::row_major_t, 10, 20>;

/// Dynamic with static rows/cols
template struct dense::detail::
 dense_matrix_base<Num, dense::detail::default_vector, dense::dynamic, 20, Int,
                   Int, dense::col_major_t, dense::dynamic, 20>;
template struct dense::detail::
 dense_matrix_base<Num, dense::detail::default_vector, dense::dynamic, 20,
                   arithmetic<unsigned, void>, arithmetic<unsigned, void>,
                   dense::row_major_t, dense::dynamic, 20>;

template struct dense::detail::
 dense_matrix_base<Num, dense::detail::default_vector, 10, dense::dynamic, Int,
                   Int, dense::col_major_t, 10, dense::dynamic>;
template struct dense::detail::
 dense_matrix_base<Num, dense::detail::default_vector, 10, dense::dynamic,
                   arithmetic<unsigned, void>, arithmetic<unsigned, void>,
                   dense::row_major_t, dense::dynamic, dense::dynamic>;

/// partially dynamic
/// todo

////////////////////////////////////////////////////////////////////////////////
/// Vector testing:

template <class T> struct unit { using value = T; };

/// List of types to test for each case:
using test_types = boost::mpl::vector<Num, Ind, Int, SInd, SInt>;

template <class Vector> void vector_test(const Ind size) {
  using T = typename Vector::value_type;
  Vector v{size};

  /// Initialize to zero:
  ranges::fill(v, T{0});
  for (Ind i = 0; i != size; ++i) { CHECK(v(i) == T{0}); }

  /// Initialize to increasing sequence starting at zero
  Ind start = 0;
  ranges::generate(v, [start]() mutable { return start++; });
  for (Ind i = 0; i != size; ++i) { CHECK(v(i) == start++); }

  /// Use Eigen expressions:
  v() = v().unaryExpr([](T i) { return i * 2; }).eval();
  start = 0;
  for (Ind i = 0; i != size; ++i) { CHECK(v(i) == (start++ * 2)); }
}

template <template <class> class Lambda> void vector_test_all(const Ind size) {
  using vectors
   = typename boost::mpl::transform<test_types,
                                    unit<Lambda<boost::mpl::_1>>>::type;

  boost::fusion::for_each(vectors{}, [size](auto v) {
    vector_test<typename decltype(v)::value>(size);
  });
}

template <class T> struct dynamic_vector {
  using type = dense::vector<T, dense::dynamic>;
};

template <class T> struct dynamic_vector_static_storage {
  using type = dense::vector<T, dense::dynamic, Ind, 20>;
};

template <class T> struct static_vector_10 {
  using type = dense::vector<T, 10>;
};

template <class T> struct static_vector_0 { using type = dense::vector<T, 0>; };

template <class T> struct static_vector_1 { using type = dense::vector<T, 1>; };

void test_vectors() {
  vector_test_all<dynamic_vector>(10);

  vector_test_all<static_vector_10>(10);
  vector_test_all<dynamic_vector_static_storage>(10);

  vector_test_all<dynamic_vector>(0);
  vector_test_all<static_vector_0>(0);
  vector_test_all<dynamic_vector_static_storage>(0);

  vector_test_all<dynamic_vector>(1);
  vector_test_all<static_vector_1>(1);
  vector_test_all<dynamic_vector_static_storage>(1);
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Matrix testing:
template <class Matrix> void matrix_test(const Ind no_rows, const Ind no_cols) {
  using T = typename Matrix::value_type;
  Matrix m{no_rows, no_cols};

  /// Initialize to zero:
  m() = Matrix::eigen_type::Zero(no_rows, no_cols);
  for (Ind r = 0; r != no_rows; ++r) {
    for (Ind c = 0; c != no_cols; ++c) { CHECK(m(r, c) == T{0}); }
  }

  // /// Initialize to increasing sequence starting at zero
  // Ind start = 0;
  // ranges::generate(v, [start]() mutable { return start++; });
  // for (Ind i = 0; i != size; ++i) { CHECK(v(i) == start++); }

  // /// Use Eigen expressions:
  // v() = v().unaryExpr([](T i) { return i * 2; }).eval();
  // start = 0;
  // for (Ind i = 0; i != size; ++i) { CHECK(v(i) == (start++ * 2)); }
}

template <template <class> class Lambda>
void matrix_test_all(const Ind no_rows, const Ind no_cols) {
  using matrices
   = typename boost::mpl::transform<test_types,
                                    unit<Lambda<boost::mpl::_1>>>::type;

  boost::fusion::for_each(matrices{}, [no_rows, no_cols](auto m) {
    matrix_test<typename decltype(m)::value>(no_rows, no_cols);
  });
}

template <class T> struct dynamic_matrix_rm_ds {
  using type
   = dense::matrix<T, dense::dynamic, dense::dynamic, Ind, Ind,
                   dense::row_major_t, dense::dynamic, dense::dynamic>;
};

template <class T> struct dynamic_matrix_cm_ds {
  using type
   = dense::matrix<T, dense::dynamic, dense::dynamic, Ind, Ind,
                   dense::col_major_t, dense::dynamic, dense::dynamic>;
};

template <class T> struct static_matrix_rm_ss_10_20 {
  using type = dense::matrix<T, 10, 20, Ind, Ind, dense::row_major_t, 10, 20>;
};

template <class T> struct static_matrix_cm_ss_10_20 {
  using type = dense::matrix<T, 10, 20, Ind, Ind, dense::col_major_t, 10, 20>;
};

template <class T> struct static_matrix_rm_ss_1_1 {
  using type = dense::matrix<T, 1, 1, Ind, Ind, dense::row_major_t, 1, 1>;
};

template <class T> struct static_matrix_cm_ss_1_1 {
  using type = dense::matrix<T, 1, 1, Ind, Ind, dense::col_major_t, 1, 1>;
};

template <class T> struct static_matrix_rm_ss_0_0 {
  using type = dense::matrix<T, 0, 0, Ind, Ind, dense::row_major_t, 0, 0>;
};

template <class T> struct static_matrix_cm_ss_0_0 {
  using type = dense::matrix<T, 0, 0, Ind, Ind, dense::col_major_t, 0, 0>;
};

void test_matrices() {
  matrix_test_all<dynamic_matrix_rm_ds>(10, 20);
  matrix_test_all<dynamic_matrix_cm_ds>(10, 20);
  matrix_test_all<dynamic_matrix_rm_ds>(1, 1);
  matrix_test_all<dynamic_matrix_cm_ds>(1, 1);
  matrix_test_all<dynamic_matrix_rm_ds>(0, 0);
  matrix_test_all<dynamic_matrix_cm_ds>(0, 0);
  matrix_test_all<dynamic_matrix_rm_ds>(1, 0);
  matrix_test_all<dynamic_matrix_cm_ds>(0, 1);
  matrix_test_all<dynamic_matrix_rm_ds>(0, 1);
  matrix_test_all<dynamic_matrix_cm_ds>(1, 0);

  matrix_test_all<static_matrix_rm_ss_10_20>(10, 20);
  matrix_test_all<static_matrix_cm_ss_10_20>(10, 20);
  matrix_test_all<static_matrix_rm_ss_1_1>(1, 1);
  matrix_test_all<static_matrix_cm_ss_1_1>(1, 1);
  matrix_test_all<static_matrix_rm_ss_0_0>(0, 0);
  matrix_test_all<static_matrix_cm_ss_0_0>(0, 0);
}

////////////////////////////////////////////////////////////////////////////////

int main() {
  test_vectors();
  return test::result();
}

////////////////////////////////////////////////////////////////////////////////
