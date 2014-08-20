#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Implements HOM3 dense matrix types
///
/// Dense matrix types with range-based and Eigen3-based interfaces.
///
/// \todo refactor element access into an access base class (see bit_matrix)
/// \todo refactor element storage into an storage base class (see bit matrix)
///
////////////////////////////////////////////////////////////////////////////////
// External includes:
#include <boost/container/vector.hpp>
// Interal includes:
#include <hom3/core/at.hpp>
#include <hom3/core/assert.hpp>
#include <hom3/core/types.hpp>
#include <hom3/core/matrix/traits.hpp>
#include <hom3/core/matrix/access.hpp>
#include <hom3/core/matrix/eigen.hpp>
////////////////////////////////////////////////////////////////////////////////

namespace hom3 {

/// Dense matrix types
namespace dense {

////////////////////////////////////////////////////////////////////////////////
/// Implementation of dense_matrix_base
namespace detail {

template <class T>
using default_vector = boost::container::vector<T, Eigen::aligned_allocator<T>>;

template <class EigenType>
using aligned_eigen_map = Eigen::Map<EigenType, Eigen::Aligned>;

template <class T, Int noRows, Int noCols, Int Order, Int maxRows = noRows,
          Int maxCols = noCols>
using aligned_eigen_type
 = Eigen::Matrix<T, noRows, noCols, Order | Eigen::AutoAlign, maxRows, maxCols>;

/// Dense matrix implementation
template <class T,                        /// value type
          template <class> class Vector,  /// container type
          Int noRows,    /// #of rows: dynamic or noRows (static)
          Int noCols,    /// #of rows: dynamic or noCols (static)
          class RowIdx,  /// Type of the index to access the rows
          class ColIdx,  /// Type of the index to access the columns
          class Order,   /// Memory layout: col_major/row_major
          Int maxRows,   /// max #of rows: dynamic or maxNoRows (static)
          Int maxCols    /// max #of columns: dynamic or maxNoCols (static)
          >
struct dense_matrix_base {};

////////////////////////////////////////////////////////////////////////////////
/// Vector, dynamic storage, dynamic size, col-major order:
/// static noCols == 1, rows == dynamic
template <class T, template <class> class Vector, class RowIdx, class ColIdx>
struct dense_matrix_base<T, Vector, dynamic, 1, RowIdx, ColIdx, col_major_t,
                         dynamic, 1> {
  using data_type = Vector<T>;
  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reference = T&;
  using const_reference = T const&;
  using value_type = T;
  using eigen_type = aligned_eigen_type<T, Eigen::Dynamic, 1, Eigen::ColMajor>;
  using eigen_map_type = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  dense_matrix_base(const Ind no_elements) : data_(no_elements) {}
  dense_matrix_base(const Ind no_rows, const Ind no_cols)
   : dense_matrix_base(no_rows * no_cols) {
    ASSERT(no_cols == 1, "error: vector with more than one column!");
  }
  dense_matrix_base() = delete;
  dense_matrix_base(dense_matrix_base const&) = default;
  dense_matrix_base(dense_matrix_base&&) = default;
  dense_matrix_base& operator=(dense_matrix_base const&) = default;
  dense_matrix_base& operator=(dense_matrix_base&&) = default;

  /// Construct from data:
  dense_matrix_base(data_type const& other) : data_{other} {}
  dense_matrix_base(data_type&& other) : data_{std::move(other)} {}

  /// Construct from view:
  template <typename Expr, int Rows, int Cols, bool I>
  dense_matrix_base(Eigen::Block<Expr, Rows, Cols, I> const& view)
   : data_{view.rows()} {
    static_assert(Cols == 1, "#of cols mismatch (vectors have 1 column)!");
    for (Ind i = 0, e = view.rows(); i != e; ++i) { data_[i] = view(i); }
  }

  /// Returns an Eigen view of the vector
  auto operator()() -> eigen_map_type {
    return eigen_map_type(data_.data(), data_.size());
  }

  /// Returns an Eigen view of the matrix
  auto operator()() const -> const_eigen_map_type {
    return const_eigen_map_type(data_.data(), data_.size());
  }

  /// Access vector elements
  const_reference operator()(const RowIdx i) const {
    return data_[index(i, size(), AT_)];
  }

  const_reference operator()(const RowIdx i, const ColIdx j) const {
    ASSERT(j == ColIdx{1}, "error: vectors have 1 column");
    return data_[index(i, size(), AT_)];
  }

  /// Access vector elements
  reference operator()(const RowIdx i) { return data_[index(i, size(), AT_)]; }

  reference operator()(const RowIdx i, const ColIdx j) {
    ASSERT(j == ColIdx{1}, "error: vectors have 1 column");
    return data_[index(i, size(), AT_)];
  }

 private:
  data_type data_;

 public:
  auto size() const RETURNS(static_cast<Int>(data_.size()));
  auto begin() RETURNS(std::begin(data_));
  auto begin() const RETURNS(data_.cbegin());
  auto end() RETURNS(std::end(data_));
  auto end() const RETURNS(data_.cend());
  auto data() RETURNS(data_.data());
  auto data() const RETURNS(data_.data());
};

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Vector, static storage, static size, col-major order:
/// noCols == 1 (static), noRows (static)
template <class T, template <class> class Vector, Int noRows, class RowIdx,
          class ColIdx>
struct dense_matrix_base<T, Vector, noRows, 1, RowIdx, ColIdx, col_major_t,
                         noRows, 1> {
  using data_type = std::array<T, noRows>;
  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reference = T&;
  using const_reference = T const&;
  using value_type = T;
  using eigen_type = aligned_eigen_type<T, noRows, 1, Eigen::ColMajor>;
  using eigen_map_type = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  dense_matrix_base(const Ind no_elements) {
    ASSERT(no_elements == noRows, "error: dimension mismatch");
  }
  dense_matrix_base(const Ind no_rows, const Ind no_cols)
   : dense_matrix_base(no_rows * no_cols) {
    ASSERT(no_cols == 1, "error: vector with more than one column!");
  }

  dense_matrix_base() = default;
  dense_matrix_base(dense_matrix_base const&) = default;
  dense_matrix_base(dense_matrix_base&&) = default;
  dense_matrix_base& operator=(dense_matrix_base const&) = default;
  dense_matrix_base& operator=(dense_matrix_base&&) = default;

  /// Construct from data:
  dense_matrix_base(data_type const& other) : data_(other) {}
  dense_matrix_base(data_type&& other) : data_(std::move(other)) {}

  /// Construct from view:
  template <typename Expr, int Rows, int Cols, bool I>
  dense_matrix_base(Eigen::Block<Expr, Rows, Cols, I> const& view) {
    static_assert(Rows == noRows, "#of rows mismatch!");
    static_assert(Cols == 1, "#of cols mismatch (vectors have 1 column)!");
    for (Ind i = 0, e = noRows; i != e; ++i) { data_[i] = view(i); }
  }

  /// Returns an Eigen view of the vector
  auto operator()() -> eigen_map_type {
    return eigen_map_type{data_.data(), noRows};
  }

  /// Returns an Eigen view of the matrix
  auto operator()() const -> const_eigen_map_type {
    return const_eigen_map_type{data_.data(), noRows};
  }

  /// Access vector elements
  const_reference operator()(const RowIdx i) const {
    return data_[index(i, size(), AT_)];
  }

  const_reference operator()(const RowIdx i, const ColIdx j) const {
    ASSERT(j == ColIdx{1}, "error: vectors have 1 column");
    return data_[index(i, size(), AT_)];
  }

  /// Access vector elements
  reference operator()(const RowIdx i) { return data_[index(i, size(), AT_)]; }

  reference operator()(const RowIdx i, const ColIdx j) {
    ASSERT(j == ColIdx{1}, "error: vectors have 1 column");
    return data_[index(i, size(), AT_)];
  }

 private:
  alignas(16) data_type data_;

 public:
  auto size() const RETURNS(noRows);
  auto begin() RETURNS(std::begin(data_));
  auto begin() const RETURNS(data_.cbegin());
  auto end() RETURNS(std::end(data_));
  auto end() const RETURNS(data_.cend());
  auto data() RETURNS(data_.data());
  auto data() const RETURNS(data_.data());
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Vector, static storage, dynamic size, col-major order:
/// noRows == dynamic, noCols == 1 (static), maxRows (static)
template <class T, template <class> class Vector, Int maxRows, class RowIdx,
          class ColIdx>
struct dense_matrix_base<T, Vector, dynamic, 1, RowIdx, ColIdx, col_major_t,
                         maxRows, 1> {
  using data_type = std::array<T, maxRows>;
  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reference = T&;
  using const_reference = T const&;
  using value_type = T;
  using eigen_type
   = aligned_eigen_type<T, Eigen::Dynamic, 1, Eigen::ColMajor, maxRows, 1>;
  using eigen_map_type = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  dense_matrix_base(const Ind noRows) : noRows_{noRows} {
    ASSERT(noRows_ <= maxRows, "error: dimension mismatch");
  }
  dense_matrix_base(const Ind no_rows, const Ind no_cols)
   : dense_matrix_base(no_rows * no_cols) {
    ASSERT(no_cols == 1, "error: vector with more than one column!");
  }

  dense_matrix_base() = delete;
  dense_matrix_base(dense_matrix_base const&) = default;
  dense_matrix_base(dense_matrix_base&&) = default;
  dense_matrix_base& operator=(dense_matrix_base const&) = default;
  dense_matrix_base& operator=(dense_matrix_base&&) = default;

  /// Construct from view:
  template <typename Expr, int Rows, int Cols, bool I>
  dense_matrix_base(Eigen::Block<Expr, Rows, Cols, I> const& view)
   : noRows_{view.rows()} {
    static_assert(Cols == 1, "#of cols mismatch (vectors have 1 column)!");
    static_assert(
     Rows == Eigen::Dynamic || Rows <= maxRows,
     "#of rows is either dynamic or static but smaller than maxRows");
    if (Rows == Eigen::Dynamic) {
      ASSERT(noRows_ <= maxRows, "dimension mismatch");
    }
    for (Ind i = 0, e = noRows_; i != e; ++i) {
      operator()(RowIdx{i}) = view(i);
    }
  }

  /// Returns an Eigen view of the vector
  auto operator()() -> eigen_map_type {
    return eigen_map_type{data_.data(), noRows_};
  }

  /// Returns an Eigen view of the matrix
  auto operator()() const -> const_eigen_map_type {
    return const_eigen_map_type{data_.data(), noRows_};
  }

  /// Access vector elements
  const_reference operator()(const RowIdx i) const {
    return data_[index(i, size(), AT_)];
  }

  const_reference operator()(const RowIdx i, const ColIdx j) const {
    ASSERT(j == ColIdx{1}, "error: vectors have 1 column");
    return data_[index(i, size(), AT_)];
  }

  /// Access vector elements
  reference operator()(const RowIdx i) { return data_[index(i, size(), AT_)]; }

  reference operator()(const RowIdx i, const ColIdx j) {
    ASSERT(j == ColIdx{1}, "error: vectors have 1 column");
    return data_[index(i, size(), AT_)];
  }

 private:
  const Ind noRows_;
  alignas(16) data_type data_;

 public:
  auto size() const RETURNS(noRows_);
  auto begin() RETURNS(std::begin(data_));
  auto begin() const RETURNS(data_.cbegin());
  auto end() RETURNS(std::begin(data_) + size());
  auto end() const RETURNS(data_.cbegin() + size());
  auto data() RETURNS(data_.data());
  auto data() const RETURNS(data_.data());
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Forbid wrong vectors: dynamic noCols, static noRows == 1
template <class T, template <class> class Vector, class RowIdx, class ColIdx,
          class Order, Int maxRows, Int maxCols>
struct dense_matrix_base<T, Vector, 1, dynamic, RowIdx, ColIdx, Order, maxRows,
                         maxCols> {};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Matrix, dynamic storage, dynamic rows, dynamic cols, row-major order:
/// noRows == dynamic, noCols == dynamic
template <class T, template <class> class Vector, class RowIdx, class ColIdx>
struct dense_matrix_base<T, Vector, dynamic, dynamic, RowIdx, ColIdx,
                         row_major_t, dynamic, dynamic> {
  using data_type = Vector<T>;
  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reference = T&;
  using const_reference = T const&;
  using value_type = T;
  using eigen_type
   = aligned_eigen_type<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor,
                        Eigen::Dynamic, Eigen::Dynamic>;
  using eigen_map_type = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  dense_matrix_base(const Ind noRows, const Ind noCols)
   : noRows_{noRows}, noCols_{noCols}, data_(noRows_ * noCols_) {}

  dense_matrix_base() = delete;
  dense_matrix_base(dense_matrix_base const&) = default;
  dense_matrix_base(dense_matrix_base&&) = default;
  dense_matrix_base& operator=(dense_matrix_base const&) = default;
  dense_matrix_base& operator=(dense_matrix_base&&) = default;

  /// Construct from view:
  template <typename Expr, int Rows, int Cols, bool I>
  dense_matrix_base(Eigen::Block<Expr, Rows, Cols, I> const& view)
   : noRows_{view.rows()}, noCols_{view.cols()}, data_(noRows_ * noCols_) {
    for (Ind c = 0, ce = noCols_; c != ce; ++c) {
      for (Ind r = 0, re = noRows_; r != re; ++r) {
        operator()(RowIdx{r}, ColIdx{c}) = view(r, c);
      }
    }
  }

  /// Returns an Eigen view of the matrix
  auto operator()() -> eigen_map_type {
    return eigen_map_type{data_.data(), noRows_, noCols_};
  }

  /// Returns an Eigen view of the matrix
  auto operator()() const -> const_eigen_map_type {
    return const_eigen_map_type{data_.data(), noRows_, noCols_};
  }

  /// Access matrix elements
  const_reference operator()(const RowIdx i, const ColIdx j) const {
    return data_[index(row_major, i, j, noRows_, noCols_, size(), AT_)];
  }

  /// Access matrix elements
  reference operator()(const RowIdx i, const ColIdx j) {
    return data_[index(row_major, i, j, noRows_, noCols_, size(), AT_)];
  }

 private:
  const Ind noRows_;
  const Ind noCols_;
  data_type data_;

 public:
  auto size() const RETURNS(static_cast<Int>(data_.size()));
  auto begin() RETURNS(std::begin(data_));
  auto begin() const RETURNS(data_.cbegin());
  auto end() RETURNS(std::begin(data_) + size());
  auto end() const RETURNS(data_.cbegin() + size());
  auto data() RETURNS(data_.data());
  auto data() const RETURNS(data_.data());
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Matrix, dynamic storage, dynamic rows, dynamic cols, col-major order:
/// noRows == dynamic, noCols == dynamic
template <class T, template <class> class Vector, class RowIdx, class ColIdx>
struct dense_matrix_base<T, Vector, dynamic, dynamic, RowIdx, ColIdx,
                         col_major_t, dynamic, dynamic> {
  using data_type = Vector<T>;
  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reference = T&;
  using const_reference = T const&;
  using value_type = T;
  using eigen_type
   = aligned_eigen_type<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor,
                        Eigen::Dynamic, Eigen::Dynamic>;
  using eigen_map_type = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  dense_matrix_base(const Ind noRows, const Ind noCols)
   : noRows_{noRows}, noCols_{noCols}, data_(noRows_ * noCols_) {}
  dense_matrix_base() = delete;
  dense_matrix_base(dense_matrix_base const&) = default;
  dense_matrix_base(dense_matrix_base&&) = default;
  dense_matrix_base& operator=(dense_matrix_base const&) = default;
  dense_matrix_base& operator=(dense_matrix_base&&) = default;

  /// Construct from view:
  template <typename Expr, int Rows, int Cols, bool I>
  dense_matrix_base(Eigen::Block<Expr, Rows, Cols, I> const& view)
   : noRows_{view.rows()}, noCols_{view.cols()}, data_(noRows_ * noCols_) {
    for (Ind r = 0, re = noRows_; r != re; ++r) {
      for (Ind c = 0, ce = noCols_; c != ce; ++c) {
        operator()(RowIdx{r}, ColIdx{c}) = view(r, c);
      }
    }
  }

  /// Returns an Eigen view of the matrix
  auto operator()() -> eigen_map_type {
    return eigen_map_type{data_.data(), noRows_, noCols_};
  }

  /// Returns an Eigen view of the matrix
  auto operator()() const -> const_eigen_map_type {
    return const_eigen_map_type{data_.data(), noRows_, noCols_};
  }

  /// Access matrix elements
  const_reference operator()(const RowIdx i, const ColIdx j) const {
    return data_[index(col_major, i, j, noRows_, noCols_, size(), AT_)];
  }

  /// Access matrix elements
  reference operator()(const RowIdx i, const ColIdx j) {
    return data_[index(col_major, i, j, noRows_, noCols_, size(), AT_)];
  }

 private:
  const Ind noRows_;
  const Ind noCols_;
  data_type data_;

 public:
  auto size() const RETURNS(static_cast<Int>(data_.size()));
  auto begin() RETURNS(std::begin(data_));
  auto begin() const RETURNS(data_.cbegin());
  auto end() RETURNS(std::begin(data_) + size());
  auto end() const RETURNS(data_.cbegin() + size());
  auto data() RETURNS(data_.data());
  auto data() const RETURNS(data_.data());
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Matrix, static storage, static rows, static cols, row-major order:
/// noRows == static, noCols == static,
template <class T, template <class> class Vector, class RowIdx, class ColIdx,
          Int noRows, Int noCols>
struct dense_matrix_base<T, Vector, noRows, noCols, RowIdx, ColIdx, row_major_t,
                         noRows, noCols> {
  using data_type = std::array<T, noRows * noCols>;
  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reference = T&;
  using const_reference = T const&;
  using value_type = T;
  using eigen_type
   = aligned_eigen_type<T, noRows, noCols, Eigen::RowMajor, noRows, noCols>;
  using eigen_map_type = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  dense_matrix_base(const Ind no_elements) {
    ASSERT(no_elements == size(), "dimension mismatch!");
  }
  dense_matrix_base(const Ind noRows_, const Ind noCols_) {
    ASSERT(noRows_ == noRows, "dimension mismatch!");
    ASSERT(noCols_ == noCols, "dimension mismatch!");
  }
  dense_matrix_base() = default;
  dense_matrix_base(dense_matrix_base const&) = default;
  dense_matrix_base(dense_matrix_base&&) = default;
  dense_matrix_base& operator=(dense_matrix_base const&) = default;
  dense_matrix_base& operator=(dense_matrix_base&&) = default;

  /// Construct from view:
  template <typename Expr, int Rows, int Cols, bool I>
  dense_matrix_base(Eigen::Block<Expr, Rows, Cols, I> const& view) {
    static_assert(Rows == noRows, "dimension mismatch!");
    static_assert(Cols == noCols, "dimension mismatch!");
    for (Ind c = 0, ce = noCols; c != ce; ++c) {
      for (Ind r = 0, re = noRows; r != re; ++r) {
        operator()(RowIdx{r}, ColIdx{c}) = view(r, c);
      }
    }
  }

  /// Returns an Eigen view of the matrix
  auto operator()() -> eigen_map_type {
    return eigen_map_type{data_.data(), noRows, noCols};
  }

  /// Returns an Eigen view of the matrix
  auto operator()() const -> const_eigen_map_type {
    return const_eigen_map_type{data_.data(), noRows, noCols};
  }

  /// Access matrix elements
  const_reference operator()(const RowIdx i, const ColIdx j) const {
    return data_[index(row_major, i, j, noRows, noCols, size(), AT_)];
  }

  /// Access matrix elements
  reference operator()(const RowIdx i, const ColIdx j) {
    return data_[index(row_major, i, j, noRows, noCols, size(), AT_)];
  }

 private:
  alignas(16) data_type data_;

 public:
  auto size() const RETURNS(noRows* noCols);
  auto begin() RETURNS(std::begin(data_));
  auto begin() const RETURNS(data_.cbegin());
  auto end() RETURNS(std::begin(data_) + size());
  auto end() const RETURNS(data_.cbegin() + size());
  auto data() RETURNS(data_.data());
  auto data() const RETURNS(data_.data());
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Matrix, static storage, static rows, static cols, col-major order:
/// noRows == static, noCols == static,
template <class T, template <class> class Vector, class RowIdx, class ColIdx,
          Int noRows, Int noCols>
struct dense_matrix_base<T, Vector, noRows, noCols, RowIdx, ColIdx, col_major_t,
                         noRows, noCols> {
  using data_type = std::array<T, noRows * noCols>;
  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reference = T&;
  using const_reference = T const&;
  using value_type = T;
  using eigen_type
   = aligned_eigen_type<T, noRows, noCols, Eigen::ColMajor, noRows, noCols>;
  using eigen_map_type = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  dense_matrix_base(const Ind no_elements) {
    ASSERT(no_elements == size(), "dimension mismatch!");
  }
  dense_matrix_base(const Ind noRows_, const Ind noCols_) {
    ASSERT(noRows_ == noRows, "dimension mismatch!");
    ASSERT(noCols_ == noCols, "dimension mismatch!");
  }
  dense_matrix_base() = default;
  dense_matrix_base(dense_matrix_base const&) = default;
  dense_matrix_base(dense_matrix_base&&) = default;
  dense_matrix_base& operator=(dense_matrix_base const&) = default;
  dense_matrix_base& operator=(dense_matrix_base&&) = default;

  /// Construct from view:
  template <typename Expr, int Rows, int Cols, bool I>
  dense_matrix_base(Eigen::Block<Expr, Rows, Cols, I> const& view) {
    static_assert(Rows == noRows, "dimension mismatch!");
    static_assert(Cols == noCols, "dimension mismatch!");
    for (Ind r = 0, re = noRows; r != re; ++r) {
      for (Ind c = 0, ce = noCols; c != ce; ++c) {
        operator()(RowIdx{r}, ColIdx{c}) = view(r, c);
      }
    }
  }

  /// Returns an Eigen view of the matrix
  auto operator()() -> eigen_map_type {
    return eigen_map_type{data_.data(), noRows, noCols};
  }

  /// Returns an Eigen view of the matrix
  auto operator()() const -> const_eigen_map_type {
    return const_eigen_map_type{data_.data(), noRows, noCols};
  }

  /// Access matrix elements
  const_reference operator()(const RowIdx i, const ColIdx j) const {
    return data_[index(col_major, i, j, noRows, noCols, size(), AT_)];
  }

  /// Access matrix elements
  reference operator()(const RowIdx i, const ColIdx j) {
    return data_[index(col_major, i, j, noRows, noCols, size(), AT_)];
  }

 private:
  alignas(16) data_type data_;

 public:
  auto size() const RETURNS(noRows* noCols);
  auto begin() RETURNS(std::begin(data_));
  auto begin() const RETURNS(data_.cbegin());
  auto end() RETURNS(std::begin(data_) + size());
  auto end() const RETURNS(data_.cbegin() + size());
  auto data() RETURNS(data_.data());
  auto data() const RETURNS(data_.data());
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Matrix, static storage, dynamic rows, dynamic cols, row-major order:
/// noRows == dynamic, noCols == dynamic, maxRows == static, maxRows == static
template <class T, template <class> class Vector, class RowIdx, class ColIdx,
          Int maxRows, Int maxCols>
struct dense_matrix_base<T, Vector, dynamic, dynamic, RowIdx, ColIdx,
                         row_major_t, maxRows, maxCols> {
  using data_type = std::array<T, maxRows * maxCols>;
  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reference = T&;
  using const_reference = T const&;
  using value_type = T;
  using eigen_type = aligned_eigen_type<T, Eigen::Dynamic, Eigen::Dynamic,
                                        Eigen::RowMajor, maxRows, maxCols>;
  using eigen_map_type = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  dense_matrix_base(const Ind noRows, const Ind noCols)
   : noRows_{noRows}, noCols_{noCols} {}
  dense_matrix_base() = default;
  dense_matrix_base(dense_matrix_base const&) = default;
  dense_matrix_base(dense_matrix_base&&) = default;
  dense_matrix_base& operator=(dense_matrix_base const&) = default;
  dense_matrix_base& operator=(dense_matrix_base&&) = default;

  /// Returns an Eigen view of the matrix
  auto operator()() -> eigen_map_type {
    return eigen_map_type{data_.data(), noRows_, noCols_};
  }

  /// Returns an Eigen view of the matrix
  auto operator()() const -> const_eigen_map_type {
    return const_eigen_map_type{data_.data(), noRows_, noCols_};
  }

  /// Access matrix elements
  const_reference operator()(const RowIdx i, const ColIdx j) const {
    return data_[index(row_major, i, j, noRows_, noCols_, size(), AT_)];
  }

  /// Access matrix elements
  reference operator()(const RowIdx i, const ColIdx j) {
    return data_[index(row_major, i, j, noRows_, noCols_, size(), AT_)];
  }

 private:
  const Ind noRows_;
  const Ind noCols_;
  alignas(16) data_type data_;

 public:
  auto size() const RETURNS(noRows_* noCols_);
  auto begin() RETURNS(std::begin(data_));
  auto begin() const RETURNS(data_.cbegin());
  auto end() RETURNS(std::begin(data_) + size());
  auto end() const RETURNS(data_.cbegin() + size());
  auto data() RETURNS(data_.data());
  auto data() const RETURNS(data_.data());
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Matrix, static storage, dynamic rows, dynamic cols, col-major order:
/// noRows == dynamic, noCols == dynamic, maxRows == static, maxRows == static
template <class T, template <class> class Vector, class RowIdx, class ColIdx,
          Int maxRows, Int maxCols>
struct dense_matrix_base<T, Vector, dynamic, dynamic, RowIdx, ColIdx,
                         col_major_t, maxRows, maxCols> {
  using data_type = std::array<T, maxRows * maxCols>;
  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reference = T&;
  using const_reference = T const&;
  using value_type = T;
  using eigen_type = aligned_eigen_type<T, Eigen::Dynamic, Eigen::Dynamic,
                                        Eigen::ColMajor, maxRows, maxCols>;
  using eigen_map_type = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  dense_matrix_base(const Ind noRows, const Ind noCols)
   : noRows_{noRows}, noCols_{noCols} {}
  dense_matrix_base() = default;
  dense_matrix_base(dense_matrix_base const&) = default;
  dense_matrix_base(dense_matrix_base&&) = default;
  dense_matrix_base& operator=(dense_matrix_base const&) = default;
  dense_matrix_base& operator=(dense_matrix_base&&) = default;

  /// Returns an Eigen view of the matrix
  auto operator()() -> eigen_map_type {
    return eigen_map_type{data_.data(), noRows_, noCols_};
  }

  /// Returns an Eigen view of the matrix
  auto operator()() const -> const_eigen_map_type {
    return const_eigen_map_type{data_.data(), noRows_, noCols_};
  }

  /// Access matrix elements
  const_reference operator()(const RowIdx i, const ColIdx j) const {
    return data_[index(col_major, i, j, noRows_, noCols_, size(), AT_)];
  }

  /// Access matrix elements
  reference operator()(const RowIdx i, const ColIdx j) {
    return data_[index(col_major, i, j, noRows_, noCols_, size(), AT_)];
  }

 private:
  const Ind noRows_;
  const Ind noCols_;
  alignas(16) data_type data_;

 public:
  auto size() const RETURNS(noRows_* noCols_);
  auto begin() RETURNS(std::begin(data_));
  auto begin() const RETURNS(data_.cbegin());
  auto end() RETURNS(std::begin(data_) + size());
  auto end() const RETURNS(data_.cbegin() + size());
  auto data() RETURNS(data_.data());
  auto data() const RETURNS(data_.data());
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Matrix, dynamic storage, dynamic rows, static cols, col_major order:
/// noRows = dynamic, noCols = static, maxRows = dynamic, maxCols = static
template <class T, Int noCols, template <class> class Vector, class RowIdx,
          class ColIdx>
struct dense_matrix_base<T, Vector, dynamic, noCols, RowIdx, ColIdx,
                         col_major_t, dynamic, noCols> {
  using data_type = Vector<T>;
  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reference = T&;
  using const_reference = T const&;
  using value_type = T;
  using eigen_type
   = aligned_eigen_type<T, Eigen::Dynamic, noCols, Eigen::ColMajor,
                        Eigen::Dynamic, noCols>;
  using eigen_map_type = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  dense_matrix_base(const Ind noRows, const Ind noCols_)
   : noRows_{noRows}, data_(noRows_ * noCols) {
    ASSERT(noCols_ == noCols, "dimension mismatch");
  }
  dense_matrix_base() = delete;
  dense_matrix_base(dense_matrix_base const&) = default;
  dense_matrix_base(dense_matrix_base&&) = default;
  dense_matrix_base& operator=(dense_matrix_base const&) = default;
  dense_matrix_base& operator=(dense_matrix_base&&) = default;

  /// Returns an Eigen view of the matrix
  auto operator()() -> eigen_map_type {
    return eigen_map_type{data_.data(), noRows_, noCols};
  }

  /// Returns an Eigen view of the matrix
  auto operator()() const -> const_eigen_map_type {
    return const_eigen_map_type{data_.data(), noRows_, noCols};
  }

  /// Access matrix elements
  const_reference operator()(const RowIdx i, const ColIdx j) const {
    return data_[index(col_major, i, j, noRows_, noCols, size(), AT_)];
  }

  /// Access matrix elements
  reference operator()(const RowIdx i, const ColIdx j) {
    return data_[index(col_major, i, j, noRows_, noCols, size(), AT_)];
  }

 private:
  const Ind noRows_;
  data_type data_;

 public:
  auto size() const RETURNS(static_cast<Int>(data_.size()));
  auto begin() RETURNS(std::begin(data_));
  auto begin() const RETURNS(data_.cbegin());
  auto end() RETURNS(std::begin(data_) + size());
  auto end() const RETURNS(data_.cbegin() + size());
  auto data() RETURNS(data_.data());
  auto data() const RETURNS(data_.data());
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Matrix, dynamic storage, dynamic rows, static cols, row_major order:
/// noRows = dynamic, noCols = static, maxRows = dynamic, maxCols = static
template <class T, Int noCols, template <class> class Vector, class RowIdx,
          class ColIdx>
struct dense_matrix_base<T, Vector, dynamic, noCols, RowIdx, ColIdx,
                         row_major_t, dynamic, noCols> {
  using data_type = Vector<T>;
  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reference = T&;
  using const_reference = T const&;
  using value_type = T;
  using eigen_type
   = aligned_eigen_type<T, Eigen::Dynamic, noCols, Eigen::RowMajor,
                        Eigen::Dynamic, noCols>;
  using eigen_map_type = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  dense_matrix_base(const Ind noRows, const Ind noCols_)
   : noRows_{noRows}, data_(noRows * noCols) {
    ASSERT(noCols_ == noCols, "dimension mismatch");
  }
  dense_matrix_base() = delete;
  dense_matrix_base(dense_matrix_base const&) = default;
  dense_matrix_base(dense_matrix_base&&) = default;
  dense_matrix_base& operator=(dense_matrix_base const&) = default;
  dense_matrix_base& operator=(dense_matrix_base&&) = default;

  /// Returns an Eigen view of the matrix
  auto operator()() -> eigen_map_type {
    return eigen_map_type{data_.data(), noRows_, noCols};
  }

  /// Returns an Eigen view of the matrix
  auto operator()() const -> const_eigen_map_type {
    return const_eigen_map_type{data_.data(), noRows_, noCols};
  }

  /// Access matrix elements
  const_reference operator()(const RowIdx i, const ColIdx j) const {
    return data_[index(row_major, i, j, noRows_, noCols, size(), AT_)];
  }

  /// Access matrix elements
  reference operator()(const RowIdx i, const ColIdx j) {
    return data_[index(row_major, i, j, noRows_, noCols, size(), AT_)];
  }

 private:
  const Ind noRows_;
  data_type data_;

 public:
  auto size() const RETURNS(static_cast<Int>(data_.size()));
  auto begin() RETURNS(std::begin(data_));
  auto begin() const RETURNS(data_.cbegin());
  auto end() RETURNS(std::begin(data_) + size());
  auto end() const RETURNS(data_.cbegin() + size());
  auto data() RETURNS(data_.data());
  auto data() const RETURNS(data_.data());
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Matrix, dynamic storage, static rows, dynamic cols, col_major order:
/// noRows = static, noCols = dynamic, maxRows = static, maxCols = dynamic
template <class T, Int noRows, template <class> class Vector, class RowIdx,
          class ColIdx>
struct dense_matrix_base<T, Vector, noRows, dynamic, RowIdx, ColIdx,
                         col_major_t, noRows, dynamic> {
  using data_type = Vector<T>;
  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reference = T&;
  using const_reference = T const&;
  using value_type = T;
  using eigen_type
   = aligned_eigen_type<T, noRows, Eigen::Dynamic, Eigen::ColMajor, noRows,
                        Eigen::Dynamic>;
  using eigen_map_type = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  dense_matrix_base(const Ind noRows_, const Ind noCols)
   : noCols_{noCols}, data_(noRows * noCols) {
    ASSERT(noRows_ == noRows, "dimension mismatch");
  }
  dense_matrix_base() = delete;
  dense_matrix_base(dense_matrix_base const&) = default;
  dense_matrix_base(dense_matrix_base&&) = default;
  dense_matrix_base& operator=(dense_matrix_base const&) = default;
  dense_matrix_base& operator=(dense_matrix_base&&) = default;

  /// Returns an Eigen view of the matrix
  auto operator()() -> eigen_map_type {
    return eigen_map_type{data_.data(), noRows, noCols_};
  }

  /// Returns an Eigen view of the matrix
  auto operator()() const -> const_eigen_map_type {
    return const_eigen_map_type{data_.data(), noRows, noCols_};
  }

  /// Access matrix elements
  const_reference operator()(const RowIdx i, const ColIdx j) const {
    return data_[index(col_major, i, j, noRows, noCols_, size(), AT_)];
  }

  /// Access matrix elements
  reference operator()(const RowIdx i, const ColIdx j) {
    return data_[index(col_major, i, j, noRows, noCols_, size(), AT_)];
  }

 private:
  const Ind noCols_;
  data_type data_;

 public:
  auto size() const RETURNS(static_cast<Int>(data_.size()));
  auto begin() RETURNS(std::begin(data_));
  auto begin() const RETURNS(data_.cbegin());
  auto end() RETURNS(std::begin(data_) + size());
  auto end() const RETURNS(data_.cbegin() + size());
  auto data() RETURNS(data_.data());
  auto data() const RETURNS(data_.data());
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Matrix, dynamic storage, static rows, dynamic cols, row_major order:
/// noRows = static, noCols = dynamic, maxRows = static, maxCols = dynamic
template <class T, Int noRows, template <class> class Vector, class RowIdx,
          class ColIdx>
struct dense_matrix_base<T, Vector, noRows, dynamic, RowIdx, ColIdx,
                         row_major_t, noRows, dynamic> {
  using data_type = Vector<T>;
  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reference = T&;
  using const_reference = T const&;
  using value_type = T;
  using eigen_type
   = aligned_eigen_type<T, noRows, Eigen::Dynamic, Eigen::ColMajor, noRows,
                        Eigen::Dynamic>;
  using eigen_map_type = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  dense_matrix_base(const Ind noRows_, const Ind noCols)
   : noCols_{noCols}, data_(noRows * noCols) {
    ASSERT(noRows_ == noRows, "dimension mismatch");
  }
  dense_matrix_base() = delete;
  dense_matrix_base(dense_matrix_base const&) = default;
  dense_matrix_base(dense_matrix_base&&) = default;
  dense_matrix_base& operator=(dense_matrix_base const&) = default;
  dense_matrix_base& operator=(dense_matrix_base&&) = default;

  /// Returns an Eigen view of the matrix
  auto operator()() -> eigen_map_type {
    return eigen_map_type{data_.data(), noRows, noCols_};
  }

  /// Returns an Eigen view of the matrix
  auto operator()() const -> const_eigen_map_type {
    return const_eigen_map_type{data_.data(), noRows, noCols_};
  }

  /// Access matrix elements
  const_reference operator()(const RowIdx i, const ColIdx j) const {
    return data_[index(row_major, i, j, noRows, noCols_, size(), AT_)];
  }

  /// Access matrix elements
  reference operator()(const RowIdx i, const ColIdx j) {
    return data_[index(row_major, i, j, noRows, noCols_, size(), AT_)];
  }

 private:
  const Ind noCols_;
  data_type data_;

 public:
  auto size() const RETURNS(static_cast<Int>(data_.size()));
  auto begin() RETURNS(std::begin(data_));
  auto begin() const RETURNS(data_.cbegin());
  auto end() RETURNS(std::begin(data_) + size());
  auto end() const RETURNS(data_.cbegin() + size());
  auto data() RETURNS(data_.data());
  auto data() const RETURNS(data_.data());
};
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \todo
/// Partial dynamic matrix of static storage: row_major, noRows static,
/// maxNoCols static
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \todo
/// Partial dynamic matrix of static storage: col_major, noRows static,
/// maxNoCols static
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \todo
/// Partial dynamic matrix of static storage: row_major, noCols static,
/// maxNoRows static
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \todo
/// Partial dynamic matrix of static storage: col_major, noCols static,
/// maxNoRows static
////////////////////////////////////////////////////////////////////////////////

}  // namespace detail

template <class T, Int noRows, class RowIdx = Ind, Int maxRows = noRows,
          template <class> class Vector = detail::default_vector>
using vector = detail::dense_matrix_base<T, Vector, noRows, 1, RowIdx, RowIdx,
                                         col_major_t, maxRows, 1>;

template <class T, Int noRows, Int noCols, class RowIdx = Ind,
          class ColIdx = Ind, class Order = col_major_t, Int maxRows = noRows,
          Int maxCols = noCols,
          template <class> class Vector = detail::default_vector>
using matrix = detail::dense_matrix_base<T, Vector, noRows, noCols, RowIdx,
                                         RowIdx, Order, maxRows, maxCols>;

}  // namespace dense
}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
