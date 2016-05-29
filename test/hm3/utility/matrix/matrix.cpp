/// \file
///
/// Test matrix class
#include <hm3/utility/matrix/matrix.hpp>
#include <hm3/utility/range.hpp>
#include <hm3/utility/test.hpp>
using namespace hm3;

////////////////////////////////////////////////////////////////////////////////
/// Explicit instantiations of the form:
/// dense::detail::matrix<T, dense::detail::default_vector,
///                                  noRows, noCols,
///                                  RowIdx, ColIdx,
///                                  order,
///                                  maxRows, maxCols>;
/// follow:

using opaque_idx = compact_optional<empty_scalar_value<int_t, -1>, struct tag>;

////////////////////////////////////////////////////////////////////////////////
/// Explicit instantiations for vector:
/// Vector, dynamic storage, dynamic size, col-major order:
/// static noCols == 1, rows == dynamic
template struct dense::matrix<num_t, dense::dynamic, 1, int_t, int_t,
                              dense::col_major_t, dense::dynamic, 1,
                              dense::default_vector>;
template struct dense::matrix<num_t, dense::dynamic, 1, opaque_idx, opaque_idx,
                              dense::col_major_t, dense::dynamic, 1,
                              dense::default_vector>;

/// Vector, static storage, static size, col-major order:
/// noCols == 1 (static), noRows (static)
template struct dense::matrix<num_t, 10, 1, int_t, int_t, dense::col_major_t,
                              10, 1, dense::default_vector>;

template struct dense::matrix<num_t, 10, 1, opaque_idx, opaque_idx,
                              dense::col_major_t, 10, 1, dense::default_vector>;

/// Vector, static storage, dynamic size, col-major order:
/// noRows == dynamic, noCols == 1 (static), maxRows (static)
template struct dense::matrix<num_t, dense::dynamic, 1, int_t, int_t,
                              dense::col_major_t, 10, 1, dense::default_vector>;

template struct dense::matrix<num_t, dense::dynamic, 1, opaque_idx, opaque_idx,
                              dense::col_major_t, 10, 1, dense::default_vector>;

////////////////////////////////////////////////////////////////////////////////
/// Explicit instantiations for matrix:

/// Fully dynamic col/row order matrix
template struct dense::matrix<num_t, dense::dynamic, dense::dynamic, int_t,
                              int_t, dense::col_major_t, dense::dynamic,
                              dense::dynamic, dense::default_vector>;
template struct dense::matrix<num_t, dense::dynamic, dense::dynamic, opaque_idx,
                              opaque_idx, dense::row_major_t, dense::dynamic,
                              dense::dynamic, dense::default_vector>;

/// Fully static col/row matrix
template struct dense::matrix<num_t,

                              10, 20, int_t, int_t, dense::col_major_t, 10, 20,
                              dense::default_vector>;
template struct dense::matrix<num_t, 10, 20, opaque_idx, opaque_idx,
                              dense::row_major_t, 10, 20,
                              dense::default_vector>;

/// Dynamic with static rows/cols
template struct dense::matrix<num_t, dense::dynamic, 20, int_t, int_t,
                              dense::col_major_t, dense::dynamic, 20,
                              dense::default_vector>;
template struct dense::matrix<num_t, dense::dynamic, 20, opaque_idx, opaque_idx,
                              dense::row_major_t, dense::dynamic, 20,
                              dense::default_vector>;

template struct dense::matrix<num_t, 10, dense::dynamic, int_t, int_t,
                              dense::col_major_t, 10, dense::dynamic,
                              dense::default_vector>;
template struct dense::matrix<num_t, 10, dense::dynamic, opaque_idx, opaque_idx,
                              dense::row_major_t, 10, dense::dynamic,
                              dense::default_vector>;

/// TODO: explicit instantiations for partially dynamic matrices

////////////////////////////////////////////////////////////////////////////////
/// Vector testing:

/// List of types to test for each case:
using test_types = meta::list<num_t, idx_t, int_t, sidx_t, sint_t>;

template <class Vector> void vector_test(const idx_t n) {
  using T = typename Vector::value_type;
  {
    Vector v(n);

    /// Initialize to zero:
    generate(v, []() { return T{0}; });
    for (idx_t i = 0; i != n; ++i) { CHECK(v(i) == T{0}); }

    /// Initialize to increasing sequence starting at zero
    idx_t start = 0;
    copy(view::iota(idx_t{0}, n), begin(v));
    for (idx_t i = 0; i != n; ++i) {
      CHECK(v(i) == start);
      start++;
    }

    /// Use Eigen expressions:
    v()   = v().unaryExpr([](T i) { return i * 2; });
    start = 0;
    for (idx_t i = 0; i != n; ++i) {
      CHECK(v(i) == (start * 2));
      start++;
    }

    /// Test range based for:
    start = 0;
    for (auto&& i : v) { CHECK(i == (start++ * 2)); }
  }
}

template <template <class> class Vector> void vector_test_all(const idx_t n) {
  meta::for_each(test_types{}, [n](auto v) {
    vector_test<meta::_t<Vector<decltype(v)>>>(n);
  });
}

template <class T> struct dynamic_vector {
  using type = dense::vector<T, dense::dynamic>;
};

template <class T> struct dynamic_vector_static_storage {
  using type = dense::vector<T, dense::dynamic, idx_t, 20>;
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
template <class Matrix>
void matrix_test(const idx_t no_rows, const idx_t no_cols) {
  using T = typename Matrix::value_type;
  Matrix m(no_rows, no_cols);

  /// Initialize to zero:
  m() = Matrix::eigen_type::Zero(no_rows, no_cols);
  for (idx_t r = 0; r != no_rows; ++r) {
    for (idx_t c = 0; c != no_cols; ++c) { CHECK(m(r, c) == T{0}); }
  }

  /// Initialize to increasing sequence starting at zero
  for (idx_t r = 0; r != no_rows; ++r) {
    for (idx_t c = 0; c != no_cols; ++c) { m(r, c) = r + c; }
  }

  /// Use Eigen expressions:
  m() = m().unaryExpr([](T i) { return i * 2; });
  for (idx_t r = 0; r != no_rows; ++r) {
    for (idx_t c = 0; c != no_cols; ++c) { CHECK(m(r, c) == (2 * (r + c))); }
  }

  Matrix m2 = m;
  Matrix m3(no_rows, no_cols);
  m3() = m2() - m();
  for (idx_t r = 0; r != no_rows; ++r) {
    for (idx_t c = 0; c != no_cols; ++c) { CHECK(m3(r, c) == T{0}); }
  }
}

template <template <class> class Matrix>
void matrix_test_all(const idx_t no_rows, const idx_t no_cols) {
  meta::for_each(test_types{}, [no_rows, no_cols](auto m) {
    matrix_test<meta::_t<Matrix<decltype(m)>>>(no_rows, no_cols);
  });
}

template <class T> struct dynamic_matrix_rm_ds {
  using type
   = dense::matrix<T, dense::dynamic, dense::dynamic, idx_t, idx_t,
                   dense::row_major_t, dense::dynamic, dense::dynamic>;
};

template <class T> struct dynamic_matrix_cm_ds {
  using type
   = dense::matrix<T, dense::dynamic, dense::dynamic, idx_t, idx_t,
                   dense::col_major_t, dense::dynamic, dense::dynamic>;
};

template <class T> struct static_matrix_rm_ss_10_20 {
  using type
   = dense::matrix<T, 10, 20, idx_t, idx_t, dense::row_major_t, 10, 20>;
};

template <class T> struct static_matrix_cm_ss_10_20 {
  using type
   = dense::matrix<T, 10, 20, idx_t, idx_t, dense::col_major_t, 10, 20>;
};

template <class T> struct static_matrix_rm_ss_1_1 {
  using type = dense::matrix<T, 1, 1, idx_t, idx_t, dense::row_major_t, 1, 1>;
};

template <class T> struct static_matrix_cm_ss_1_1 {
  using type = dense::matrix<T, 1, 1, idx_t, idx_t, dense::col_major_t, 1, 1>;
};

template <class T> struct static_matrix_rm_ss_0_0 {
  using type = dense::matrix<T, 0, 0, idx_t, idx_t, dense::row_major_t, 0, 0>;
};

template <class T> struct static_matrix_cm_ss_0_0 {
  using type = dense::matrix<T, 0, 0, idx_t, idx_t, dense::col_major_t, 0, 0>;
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

int main() {
  test_vectors();

  // test initialization list:
  dense::vector<int, 1> v1{7};
  dense::vector<int, 3> v2{2, 5, 6};

  CHECK(v1(0) == 7);
  CHECK(v2(0) == 2);
  CHECK(v2(1) == 5);
  CHECK(v2(2) == 6);

  return test::result();
}
