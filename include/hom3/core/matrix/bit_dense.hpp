#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Implements HOM3 bit dense matrix types
///
////////////////////////////////////////////////////////////////////////////////
/// External includes:
#include <boost/dynamic_bitset.hpp>
/// Internal includes:
#include <hom3/core/at.hpp>
#include <hom3/core/assert.hpp>
#include <hom3/core/types.hpp>
#include <hom3/core/matrix/traits.hpp>
#include <hom3/core/matrix/access.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace hom3 {

namespace dense {

namespace detail {

/// Storage class for fully static bit matrix
template <Int nRows, Int nCols> struct bit_storage {
  using type = bit_storage<nRows, nCols>;
  using data_type = std::bitset<nRows * nCols>;
  using reference = typename data_type::reference;

  bit_storage(Ind nRows_, Ind nCols_) {
    ASSERT(nRows_ == nRows, "dimension mismatch");
    ASSERT(nCols_ == nCols, "dimension mismatch");
  }
  bit_storage() = default;
  bit_storage(type const&) = default;
  bit_storage(type&&) = default;
  type& operator=(type const&) = default;
  type& operator=(type&&) = default;

 protected:
  data_type data_;

 public:
  auto size() const RETURNS(static_cast<Int>(data_.size()));
  auto nrows() const RETURNS(nRows);
  auto ncols() const RETURNS(nCols);
};

/// Storage class for dynamic bit matrix with constant nRows
template <Int nRows> struct bit_storage<nRows, dynamic> {
  using type = bit_storage<nRows, dynamic>;
  using data_type = boost::dynamic_bitset<>;
  using reference = typename data_type::reference;

  bit_storage() = delete;
  bit_storage(type const&) = default;
  bit_storage(type&&) = default;
  type& operator=(type const&) = default;
  type& operator=(type&&) = default;
  bit_storage(const Ind nRows_, const Ind nCols)
   : nCols_{nCols}, data_{ncols() * nrows()} {
    ASSERT(nRows_ == nrows(), "dimension mismatch");
  }

 protected:
  const Int nCols_;
  data_type data_;

 public:
  auto size() const RETURNS(static_cast<Int>(data_.size()));
  auto nrows() const RETURNS(nRows);
  auto ncols() const RETURNS(nCols_);
};

/// Storage class for dynamic bit matrix with constant nCols
template <Int nCols> struct bit_storage<dynamic, nCols> {
  using type = bit_storage<dynamic, nCols>;
  using data_type = boost::dynamic_bitset<unsigned long long>;
  using reference = typename data_type::reference;

  bit_storage() = delete;
  bit_storage(type const&) = default;
  bit_storage(type&&) = default;
  type& operator=(type const&) = default;
  type& operator=(type&&) = default;
  bit_storage(const Int nr, const Int nc) : nRows_{nr}, data_{nCols * nRows_} {
    ASSERT(nc == nCols, "dimension mismatch");
  }

 protected:
  const Int nRows_;
  data_type data_;

 public:
  auto size() const RETURNS(static_cast<Int>(data_.size()));
  auto nrows() const RETURNS(nRows_);
  auto ncols() const RETURNS(nCols);
};

/// Storage class for fully dynamic bit matrix:
template <> struct bit_storage<dynamic, dynamic> {
  using type = bit_storage<dynamic, dynamic>;
  using data_type = boost::dynamic_bitset<>;
  using reference = typename data_type::reference;

  bit_storage() = delete;
  bit_storage(type const&) = default;
  bit_storage(type&&) = default;
  type& operator=(type const&) = default;
  type& operator=(type&&) = default;
  bit_storage(const Int nr, const Int nc)
   : nRows_{nr}, nCols_{nc}, data_(nCols_ * nRows_) {}

 protected:
  const Int nRows_;
  const Int nCols_;
  data_type data_;

 public:
  auto size() const RETURNS(static_cast<Int>(data_.size()));
  auto nrows() const RETURNS(nRows_);
  auto ncols() const RETURNS(nCols_);
};

/// Storage class for dynamic bit vector
template <> struct bit_storage<dynamic, 1> {
  using type = bit_storage<dynamic, 1>;
  using data_type = boost::dynamic_bitset<unsigned long long>;
  using reference = typename data_type::reference;

  bit_storage() = delete;
  bit_storage(type const&) = default;
  bit_storage(type&&) = default;
  type& operator=(type const&) = default;
  type& operator=(type&&) = default;
  bit_storage(const Int nr) : data_(nr) {}
  void resize(std::size_t num_bits, bool value = false) {
    data_.resize(num_bits, value);
  }

 protected:
  data_type data_;

 public:
  auto size() const RETURNS(static_cast<Int>(data_.size()));
  auto nrows() const RETURNS(size());
  auto ncols() const RETURNS(Int{1});
};

/// Storage class for fully static bit vector
template <Int nRows> struct bit_storage<nRows, 1> {
  using type = bit_storage<nRows, 1>;
  using data_type = std::bitset<nRows>;
  using reference = typename data_type::reference;

  bit_storage(Ind nRows_) { ASSERT(nRows_ == nRows, "dimension mismatch"); }
  bit_storage() = default;
  bit_storage(type const&) = default;
  bit_storage(type&&) = default;
  type& operator=(type const&) = default;
  type& operator=(type&&) = default;

 protected:
  data_type data_;

 public:
  auto size() const RETURNS(nRows);
  auto nrows() const RETURNS(nRows);
  auto ncols() const RETURNS(Int{1});
};

template <Int nRows, Int nCols, int Order, class RowIdx, class ColIdx>
struct bit_access : bit_storage<nRows, nCols> {
  using ERROR = typename bit_storage<nRows, nCols>::Unknown_Access_Order;
};

template <Int nRows, Int nCols, class RowIdx, class ColIdx>
struct bit_access<nRows, nCols, Eigen::RowMajor, RowIdx, ColIdx>
 : bit_storage<nRows, nCols> {
  using reference = typename bit_storage<nRows, nCols>::reference;
  using data_type = typename bit_storage<nRows, nCols>::data_type;
  using bit_storage<nRows, nCols>::bit_storage;
  using bit_storage<nRows, nCols>::operator=;
  reference operator()(const RowIdx r, const ColIdx c) noexcept {
    return this->data_[index(row_major, r, c, this->nrows(), this->ncols(),
                             this->size(), AT_)];
  }
  bool operator()(const RowIdx r, const ColIdx c) const noexcept {
    return this->data_[index(row_major, r, c, this->nrows(), this->ncols(),
                             this->size(), AT_)];
  }
  data_type& data() { return this->data_; }
  data_type const& data() const { return this->data_; }
};

template <Int nRows, Int nCols, class RowIdx, class ColIdx>
struct bit_access<nRows, nCols, Eigen::ColMajor, RowIdx, ColIdx>
 : bit_storage<nRows, nCols> {
  using bit_storage<nRows, nCols>::bit_storage;
  using bit_storage<nRows, nCols>::operator=;
  using reference = typename bit_storage<nRows, nCols>::reference;
  using data_type = typename bit_storage<nRows, nCols>::data_type;
  reference operator()(const RowIdx r, const ColIdx c = ColIdx{0}) noexcept {
    return this->data_[index(col_major, r, c, this->nrows(), this->ncols(),
                             this->size(), AT_)];
  }
  bool operator()(const RowIdx r, const ColIdx c = ColIdx{0}) const noexcept {
    return this->data_[index(col_major, r, c, this->nrows(), this->ncols(),
                             this->size(), AT_)];
  }
  data_type& data() { return this->data_; }
  data_type const& data() const { return this->data_; }
};

template <Int nRows, Int nCols, int Order, class RowIdx, class ColIdx>
struct bit_matrix_base : bit_access<nRows, nCols, Order, RowIdx, ColIdx> {
  using type = bit_matrix_base<nRows, nCols, Order, RowIdx, ColIdx>;
  using reference
   = typename bit_access<nRows, nCols, Order, RowIdx, ColIdx>::reference;
  using bit_access<nRows, nCols, Order, RowIdx, ColIdx>::bit_access;
  using bit_access<nRows, nCols, Order, RowIdx, ColIdx>::operator=;

  type& set() {
    this->data_.set();
    return *this;
  }
  type& reset() {
    this->data_.reset();
    return *this;
  }
};

}  // namespace detail

template <Int nRows, Int nCols, int Order = Eigen::ColMajor, class RowIdx = Ind,
          class ColIdx = SInd>
struct bit_matrix
 : detail::bit_matrix_base<nRows, nCols, Order, RowIdx, ColIdx> {
  using base = detail::bit_matrix_base<nRows, nCols, Order, RowIdx, ColIdx>;
  using base::base;
  using base::operator=;
  using reference = typename base::reference;
};

template <Int ro, Int co, int o, class ri, class ci>
inline bool operator==(bit_matrix<ro, co, o, ri, ci> const& l,
                       bit_matrix<ro, co, o, ri, ci> const& r) {
  return l.data() == r.data();
}

template <Int noRows, class RowIdx = Ind>
using bit_vector = bit_matrix<noRows, 1, Eigen::ColMajor, RowIdx, RowIdx>;

}  // namespace dense
}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
