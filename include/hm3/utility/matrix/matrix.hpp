#pragma once
/// \file
///
/// Dense matrix implementation
///
/// TODO: when returning an eigen view of the matrix using operator() return an
/// EigenMap only when the matrix is an lvalue (& or const&), and return an
/// EigenMatrix when the matrix is an rvalue (&&).
#include <hm3/utility/matrix/access.hpp>
#include <hm3/utility/matrix/storage.hpp>
#include <hm3/utility/unwrap.hpp>

namespace hm3 {

/// Dense matrix
namespace dense {

/// Dense matrix type
template <typename T,                    /// value type
          int_t NoRows,                  /// #of rows (number/dynamic)
          int_t NoCols,                  /// #of cols  (number/dynamic)
          typename RowIdx = idx_t,       /// row index
          typename ColIdx = idx_t,       /// column index
          typename Order = col_major_t,  /// memory layout (col/row-major-order)
          int_t MaxRows  = NoRows,       /// max #of rows (number/dynamic)
          int_t MaxCols  = NoCols,       /// max #of columns (number/dynamic)
          template <class> class Vector  /// container type
          = default_vector               /// default
          >
struct matrix : bounds<NoRows, NoCols>,
                storage<T, Vector, NoRows, NoCols, MaxRows, MaxCols,
                        storage_type_t<MaxRows, MaxCols>> {
  using storage = storage<T, Vector, NoRows, NoCols, MaxRows, MaxCols,
                          storage_type_t<MaxRows, MaxCols>>;
  using bounds = bounds<NoRows, NoCols>;

  // Either: NoRows is not dynamic and MaxRows equals NoRows
  static_assert(((NoRows != dynamic) && (NoRows == MaxRows))
                 || NoRows == dynamic,  // or NoRows is dynamic
                "if NoRows != dynamic, then MaxRows must equal NoRows");
  // Either: NoCols is not dynamic and MaxCols equals NoCols
  static_assert(((NoCols != dynamic) && (NoCols == MaxCols))
                 || NoCols == dynamic,  // or NoCols is dynamic
                "if NoCols != dynamic, then MaxCols must equal NoCols");

  static_assert(std::is_same<Order, row_major_t>{}
                 || std::is_same<Order, col_major_t>{},
                "unknow matrix ordering");

  using storage::max_no_rows;
  using storage::max_no_cols;
  using storage::max_size;
  using bounds::no_rows;
  using bounds::no_cols;
  using bounds::size;
  using storage::container;
  using data_container  = typename storage::data_container;
  using reference       = typename storage::reference;
  using const_reference = typename storage::const_reference;

  using eigen_type
   = aligned_eigen_type<T, NoRows, NoCols, to_eigen_order<Order>::value>;
  using eigen_map_type       = aligned_eigen_map<eigen_type>;
  using const_eigen_map_type = aligned_eigen_map<const eigen_type>;

  static constexpr bool is_vector() { return NoCols == 1 && MaxCols == 1; }
  static constexpr bool is_fixed() {
    return NoCols != dynamic && NoRows != dynamic;
  }
  static constexpr bool is_bit() { return std::is_same<T, bit>::value; }

  constexpr matrix() = default;

  constexpr matrix(matrix const&) = default;
  constexpr matrix(matrix&&)      = default;

  constexpr matrix& operator=(matrix const&) = default;
  constexpr matrix& operator=(matrix&&) = default;

  // template <typename Int,
  //           CONCEPT_REQUIRES_(is_vector() and std::is_integral<Int>{})>
  CONCEPT_REQUIRES(is_vector())
  explicit constexpr matrix(const uint_t no_elements)
   : bounds(no_elements, 1), storage(no_elements) {
    HM3_MATRIX_ASSERT_NO_ELEMENTS_CONSTRUCTOR;
  }

  template <typename U,
            CONCEPT_REQUIRES_(is_vector() and NoRows == 1 and NoCols == 1
                              and !Same<uncvref_t<U>, uint_t>{}
                              and ranges::Assignable<U, T>{})>
  explicit constexpr matrix(U value) : bounds(1, 1), storage(1) {
    operator()(static_cast<RowIdx>(0)) = std::move(value);
  }

  CONCEPT_REQUIRES(is_vector())
  constexpr matrix(std::initializer_list<T> args)
   : bounds(args.size(), 1), storage(std::move(args)) {}

  explicit constexpr matrix(const uint_t no_rows_, const uint_t no_cols_)
   : bounds(no_rows_, no_cols_), storage(no_rows_, no_cols_) {}

  template <
   typename I, typename U = uncvref_t<I>,
   CONCEPT_REQUIRES_(
    is_vector()
    && std::is_same<typename std::iterator_traits<U>::value_type, T>{})>
  explicit constexpr matrix(I begin, I end)
   : bounds(end - begin, 1), storage(end - begin, 1) {
    for (uint_t i = 0, e = no_rows(); i != e; ++i) {
      operator()(static_cast<RowIdx>(i)) = *(begin + i);
    }
  }

  template <typename Other>
  constexpr void element_wise_assign(Other&& o) {
    if (std::is_same<Order, col_major_t>{}) {
      for (uint_t c = 0, ce = no_cols(); c != ce; ++c) {
        for (uint_t r = 0, re = no_rows(); r != re; ++r) {
          operator()(static_cast<RowIdx>(r), static_cast<ColIdx>(c)) = o(r, c);
        }
      }
    } else {  // row major
      for (uint_t r = 0, re = no_rows(); r != re; ++r) {
        for (uint_t c = 0, ce = no_cols(); c != ce; ++c) {
          operator()(static_cast<RowIdx>(r), static_cast<ColIdx>(c)) = o(r, c);
        }
      }
    }
  }

  /// Construct from Eigen expression:
  template <typename Expr, int Rows, int Cols, bool I,
            CONCEPT_REQUIRES_(!is_bit())>
  explicit constexpr matrix(Eigen::Block<Expr, Rows, Cols, I> const& view)
   : bounds(view.rows(), view.cols()), storage(view.rows(), view.cols()) {
    HM3_ASSERT(Rows == no_rows() && Cols == no_cols(),
               "dimension mismatch | rows (runtime: {}, compile-time: {}) | "
               "cols (runtime: {}, compile-time: {})!",
               no_rows(), Rows, no_cols(), Cols);
    element_wise_assign(view);
  }

  /// Construct from Eigen expression:
  template <typename Expr, int Rows, int Cols, bool I,
            CONCEPT_REQUIRES_(!is_bit())>
  constexpr matrix& operator=(Eigen::Block<Expr, Rows, Cols, I> const& view) {
    HM3_ASSERT(Rows == no_rows() && Cols == no_cols(),
               "dimension mismatch | rows (runtime: {}, compile-time: {}) | "
               "cols (runtime: {}, compile-time: {})!",
               no_rows(), Rows, no_cols(), Cols);
    element_wise_assign(view);
    return *this;
  }

  /// Construct from Eigen expression:
  CONCEPT_REQUIRES(!is_bit())  // this can be relaxed
  constexpr matrix(Eigen::Matrix<T, NoRows, NoCols> const& other)
   : storage(other.rows(), other.cols()) {
    element_wise_assign(other);
  }

  /// Construct from Eigen expression:
  CONCEPT_REQUIRES(!is_bit())  // this can be relaxed
  constexpr matrix& operator=(Eigen::Matrix<T, NoRows, NoCols> const& other) {
    element_wise_assign(other);
    return *this;
  }

  /// Returns an Eigen view of the matrix
  CONCEPT_REQUIRES(!is_bit())
  constexpr auto operator()() & -> eigen_map_type {
    return eigen_map_type{this->data(), static_cast<int_t>(no_rows()),
                          static_cast<int_t>(no_cols())};
  }

  CONCEPT_REQUIRES(!is_bit())
  constexpr auto operator()() && -> eigen_map_type = delete;

  /// Returns an Eigen view of the matrix
  CONCEPT_REQUIRES(!is_bit())
  constexpr auto operator()() const & -> const_eigen_map_type {
    return const_eigen_map_type{this->data(), static_cast<int_t>(no_rows()),
                                static_cast<int_t>(no_cols())};
  }

  /// Access vector elements
  CONCEPT_REQUIRES(is_vector())
  constexpr const_reference operator()(const RowIdx index) const & {
    const auto i = unwrap(index);
    HM3_ASSERT(static_cast<std::size_t>(i) < size(),
               "vector index '{}' is out-of-range [0, {})", i, size());
    return container()[i];
  }

  CONCEPT_REQUIRES(is_vector())
  constexpr T operator()(const RowIdx index) && {
    auto i = unwrap(index);
    HM3_ASSERT(static_cast<std::size_t>(i) < size(),
               "vector index '{}' is out-of-range [0, {})", i, size());
    return container()[i];
  }

  /// Access vector elements
  CONCEPT_REQUIRES(is_vector())
  constexpr reference operator()(const RowIdx index) & {
    const auto i = unwrap(index);
    HM3_ASSERT(static_cast<std::size_t>(i) < size(),
               "vector index '{}' is out-of-range [0, {})", i, size());
    return container()[i];
  }

  /// Access vector elements
  CONCEPT_REQUIRES(is_vector())
  constexpr const_reference operator[](const RowIdx index) const & {
    const auto i = unwrap(index);
    HM3_ASSERT(static_cast<std::size_t>(i) < size(),
               "vector index '{}' is out-of-range [0, {})", i, size());
    return container()[i];
  }

  CONCEPT_REQUIRES(is_vector())
  constexpr const_reference operator[](const RowIdx) && = delete;

  /// Access vector elements
  CONCEPT_REQUIRES(is_vector())
  constexpr reference operator[](const RowIdx index) & {
    const auto i = unwrap(index);
    HM3_ASSERT(static_cast<std::size_t>(i) < size(),
               "vector index '{}' is out-of-range [0, {})", i, size());
    return container()[i];
  }

  CONCEPT_REQUIRES(!is_bit())
  static constexpr auto constant(T value) noexcept {
    return matrix{eigen_map_type::Constant(value)};
  }

  CONCEPT_REQUIRES(!is_bit())
  static constexpr auto unit(suint_t d) noexcept {
    return matrix{eigen_map_type::Unit(d)};
  }

  CONCEPT_REQUIRES(!is_bit())
  static constexpr auto c(T value) noexcept {
    return eigen_map_type::Constant(value);
  }

  CONCEPT_REQUIRES(!is_bit())
  static constexpr auto zero() noexcept {
    return matrix{eigen_map_type::Zero()};
  }

  CONCEPT_REQUIRES(!is_bit())
  static constexpr auto ones() noexcept {
    return matrix{eigen_map_type::Ones()};
  }

  CONCEPT_REQUIRES(!is_bit())
  static constexpr auto linspace(std::size_t size, T low, T high) noexcept {
    return matrix{eigen_map_type::LinSpaced(size, low, high)};
  }

 public:
  /// Access matrix elements
  constexpr const_reference operator()(const RowIdx row,
                                       const ColIdx col) const {
    const auto r = unwrap(row);
    const auto c = unwrap(col);
    HM3_ASSERT(static_cast<uint_t>(r) < no_rows(),
               "row index '{}' is out-of-range: [0, {})", r, no_rows());
    HM3_ASSERT(static_cast<uint_t>(c) < no_cols(),
               "col index '{}' is out-of-range: [0, {})", c, no_cols());
    return container()[detail::matrix_idx(r, c, max_no_rows(), max_no_cols(),
                                          Order{})];
  }
  constexpr reference operator()(const RowIdx row, const ColIdx col) {
    const auto r = unwrap(row);
    const auto c = unwrap(col);
    HM3_ASSERT(static_cast<uint_t>(r) < no_rows(),
               "row index '{}' is out-of-range: [0, {})", r, no_rows());
    HM3_ASSERT(static_cast<uint_t>(c) < no_cols(),
               "col index '{}' is out-of-range: [0, {})", c, no_cols());
    return container()[detail::matrix_idx(r, c, max_no_rows(), max_no_cols(),
                                          Order{})];
  }

  CONCEPT_REQUIRES(!is_bit())
  auto row(const RowIdx r) { return (*this)().row(unwrap(r)); }
  CONCEPT_REQUIRES(!is_bit())
  auto row(const RowIdx r) const { return (*this)().row(unwrap(r)); }
  CONCEPT_REQUIRES(!is_bit())
  auto col(const ColIdx c) { return (*this)().col(unwrap(c)); }
  CONCEPT_REQUIRES(!is_bit())
  auto col(const ColIdx c) const { return (*this)().col(unwrap(c)); }

  auto begin() noexcept { return ranges::begin(this->data_); }
  auto begin() const noexcept { return ranges::begin(this->data_); }
  auto end() noexcept { return ranges::begin(this->data_) + size(); }
  auto end() const noexcept { return ranges::begin(this->data_) + size(); }

 public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#pragma clang diagnostic pop
};

template <
 typename T, int_t NoRows, int_t NoCols, typename RowIdx = idx_t,
 typename ColIdx = idx_t, typename Order = col_major_t, int_t MaxRows = NoRows,
 int_t MaxCols = NoCols, template <class> class Vector,
 typename M
 = matrix<T, NoRows, NoCols, RowIdx, ColIdx, Order, MaxRows, MaxCols, Vector>,
 CONCEPT_REQUIRES_(M::is_vector() && !M::is_bit())>
constexpr auto begin(matrix<T, NoRows, NoCols, RowIdx, ColIdx, Order, MaxRows,
                            MaxCols, Vector>& m) noexcept {
  return ranges::begin(m.data_);
}

template <
 typename T, int_t NoRows, int_t NoCols, typename RowIdx = idx_t,
 typename ColIdx = idx_t, typename Order = col_major_t, int_t MaxRows = NoRows,
 int_t MaxCols = NoCols, template <class> class Vector,
 typename M
 = matrix<T, NoRows, NoCols, RowIdx, ColIdx, Order, MaxRows, MaxCols, Vector>,
 CONCEPT_REQUIRES_(M::is_vector() && !M::is_bit())>
constexpr auto end(matrix<T, NoRows, NoCols, RowIdx, ColIdx, Order, MaxRows,
                          MaxCols, Vector>& m) noexcept {
  return ranges::begin(m.data_) + m.size();
}

template <
 typename T, int_t NoRows, int_t NoCols, typename RowIdx = idx_t,
 typename ColIdx = idx_t, typename Order = col_major_t, int_t MaxRows = NoRows,
 int_t MaxCols = NoCols, template <class> class Vector,
 typename M
 = matrix<T, NoRows, NoCols, RowIdx, ColIdx, Order, MaxRows, MaxCols, Vector>,
 CONCEPT_REQUIRES_(M::is_vector() && !M::is_bit())>
constexpr auto begin(matrix<T, NoRows, NoCols, RowIdx, ColIdx, Order, MaxRows,
                            MaxCols, Vector> const& m) noexcept {
  return ranges::begin(m.data_);
}

template <
 typename T, int_t NoRows, int_t NoCols, typename RowIdx = idx_t,
 typename ColIdx = idx_t, typename Order = col_major_t, int_t MaxRows = NoRows,
 int_t MaxCols = NoCols, template <class> class Vector,
 typename M
 = matrix<T, NoRows, NoCols, RowIdx, ColIdx, Order, MaxRows, MaxCols, Vector>,
 CONCEPT_REQUIRES_(M::is_vector() && !M::is_bit())>
constexpr auto end(matrix<T, NoRows, NoCols, RowIdx, ColIdx, Order, MaxRows,
                          MaxCols, Vector> const& m) noexcept {
  return ranges::begin(m.data_) + m.size();
}

template <
 typename T, int_t NoRows, int_t NoCols, typename RowIdx = idx_t,
 typename ColIdx = idx_t, typename Order = col_major_t, int_t MaxRows = NoRows,
 int_t MaxCols = NoCols, template <class> class Vector,
 typename M
 = matrix<T, NoRows, NoCols, RowIdx, ColIdx, Order, MaxRows, MaxCols, Vector>,
 CONCEPT_REQUIRES_(M::is_vector() && !M::is_bit())>
constexpr auto cbegin(matrix<T, NoRows, NoCols, RowIdx, ColIdx, Order, MaxRows,
                             MaxCols, Vector> const& m) noexcept {
  return m.data_.cbegin();
}

template <
 typename T, int_t NoRows, int_t NoCols, typename RowIdx = idx_t,
 typename ColIdx = idx_t, typename Order = col_major_t, int_t MaxRows = NoRows,
 int_t MaxCols = NoCols, template <class> class Vector,
 typename M
 = matrix<T, NoRows, NoCols, RowIdx, ColIdx, Order, MaxRows, MaxCols, Vector>,
 CONCEPT_REQUIRES_(M::is_vector() && !M::is_bit())>
constexpr auto cend(matrix<T, NoRows, NoCols, RowIdx, ColIdx, Order, MaxRows,
                           MaxCols, Vector> const& m) noexcept {
  return m.data_.cbegin() + m.size();
}

template <typename T, int_t NoRows, typename RowIdx = idx_t,
          int_t MaxRows                 = NoRows,
          template <class> class Vector = default_vector>
using vector = matrix<T, NoRows, 1, RowIdx, RowIdx, col_major_t, MaxRows, 1>;

}  // namespace dense
}  // namespace hm3
