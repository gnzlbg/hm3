#pragma once
/// \file
///
/// Dense matrix storage
#include <bitset>
#include <boost/container/vector.hpp>
#include <boost/dynamic_bitset.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/matrix/bounds.hpp>
#include <hm3/utility/matrix/traits.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace dense {

#define HM3_MATRIX_ASSERT_NO_ELEMENTS_CONSTRUCTOR                            \
  HM3_ASSERT(no_elements <= max_size(), "#of elements: '{}' should be <= "   \
                                        "max size '{}' (max_no_rows: '{}', " \
                                        "max_no_cols: '{}')",                \
             no_elements, max_size(), max_no_rows(), max_no_cols())

#define HM3_MATRIX_ASSERT_NO_ROWS_AND_COLS_CONSTRUCTOR                 \
  HM3_ASSERT(no_rows_ <= max_no_rows(),                                \
             "#of rows: '{}' should be <= max_no_rows '{}'", no_rows_, \
             max_no_rows());                                           \
  HM3_ASSERT(no_cols_ <= max_no_cols(),                                \
             "#of cols: '{}' should be <= max_no_cols '{}'", no_cols_, \
             max_no_cols())

template <typename T>
using default_vector = boost::container::vector<T, Eigen::aligned_allocator<T>>;

template <typename T, template <class> class StorageContainer, int_t NoRows,
          int_t NoCols, int_t MaxRows, int_t MaxCols, typename StorageType>
struct storage {};

/// Stack storage implementation
///
/// \pre MaxRows != dynamic && MaxCols != dynamic
template <typename T, template <class> class StorageContainer, int_t NoRows,
          int_t NoCols, int_t MaxRows, int_t MaxCols>
struct storage<T, StorageContainer, NoRows, NoCols, MaxRows, MaxCols,
               stack_storage> : max_bounds<MaxRows, MaxCols> {
  static_assert(MaxRows != dynamic, "stack storage with dynamic #of rows");
  static_assert(MaxCols != dynamic, "stack storage with dynamic #of columns");
  static_assert(NoRows <= MaxRows, "NoRows <= MaxRows!");
  static_assert(NoCols <= MaxCols, "NoCols <= MaxCols!");

  using max_bounds = max_bounds<MaxRows, MaxCols>;
  using max_bounds::max_no_rows;
  using max_bounds::max_no_cols;
  using max_bounds::max_size;

  using data_container  = array<T, (max_size() > 0) ? max_size() : 1>;
  using iterator        = typename data_container::iterator;
  using const_iterator  = typename data_container::const_iterator;
  using reference       = typename data_container::reference;
  using const_reference = typename data_container::const_reference;
  using value_type      = typename data_container::value_type;

  // using data_container  = Eigen::Matrix<T, max_size(), 1>;
  // using iterator        = T*;
  // using const_iterator  = T const*;
  // using reference       = T&;
  // using const_reference = T const&;
  // using value_type = T;

  using pointer       = value_type*;
  using const_pointer = value_type const*;

  alignas(64) data_container data_;

  constexpr storage()               = default;
  constexpr storage(storage const&) = default;

  constexpr storage(storage&&) = default;
  constexpr storage& operator=(storage const&) = default;
  constexpr storage& operator=(storage&&) = default;

  constexpr storage(data_container const& other) : data_(other) {}
  constexpr storage(data_container&& other) : data_(std::move(other)) {}

  constexpr storage(const uint_t no_elements) {
    HM3_MATRIX_ASSERT_NO_ELEMENTS_CONSTRUCTOR;
  }

  constexpr storage(const uint_t no_rows_, const uint_t no_cols_) {
    HM3_MATRIX_ASSERT_NO_ROWS_AND_COLS_CONSTRUCTOR;
  }

 private:
  template <std::size_t... Is>
  constexpr storage(std::index_sequence<Is...>, std::initializer_list<T> v)
   : data_{{v.begin()[Is]...}} {}

 public:
  constexpr storage(std::initializer_list<T> v)
   : storage(std::make_index_sequence<MaxRows * MaxCols>{}, v) {}

  constexpr auto container() -> data_container& { return data_; }
  constexpr auto container() const -> data_container const& { return data_; }

  constexpr auto data() -> pointer { return &data_[0]; }
  constexpr auto data() const -> const_pointer { return &data_[0]; }

 public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#pragma clang diagnostic pop
};

/// Dynamic storage
///
/// \pre MaxRows == dynamic || MaxCols == dynamic
template <typename T, template <class> class StorageContainer, int_t NoRows,
          int_t NoCols, int_t MaxRows, int_t MaxCols>
struct storage<T, StorageContainer, NoRows, NoCols, MaxRows, MaxCols,
               dynamic_storage> : max_bounds<MaxRows, MaxCols> {
  static_assert(MaxRows == dynamic || MaxCols == dynamic,
                "static MaxRows and MaxCols: use stack storage!");

  using data_container  = StorageContainer<T>;
  using iterator        = typename data_container::iterator;
  using const_iterator  = typename data_container::const_iterator;
  using reference       = typename data_container::reference;
  using const_reference = typename data_container::const_reference;
  using value_type      = typename data_container::value_type;
  using pointer         = value_type*;
  using const_pointer   = value_type const*;

  alignas(32) data_container data_;

  using max_bounds = max_bounds<MaxRows, MaxCols>;
  using max_bounds::max_no_rows;
  using max_bounds::max_no_cols;
  using max_bounds::max_size;

  storage()               = default;
  storage(storage const&) = default;
  storage(storage&&)      = default;
  storage& operator=(storage const&) = default;
  storage& operator=(storage&&) = default;

  CONCEPT_REQUIRES(MaxRows == dynamic && MaxCols == 1)
  storage(const uint_t no_elements)
   : max_bounds(no_elements, 1), data_(max_size()) {
    HM3_MATRIX_ASSERT_NO_ELEMENTS_CONSTRUCTOR;
  }

  storage(const uint_t no_rows_, const uint_t no_cols_)
   : max_bounds(no_rows_, no_cols_), data_(max_size()) {
    HM3_MATRIX_ASSERT_NO_ROWS_AND_COLS_CONSTRUCTOR;
  }

  CONCEPT_REQUIRES(MaxRows == dynamic && MaxCols == 1)
  storage(std::initializer_list<T> args)
   : max_bounds(args.size(), 1), data_(max_size()) {
    HM3_ASSERT(args.size() == max_size(),
               "mismatch #of elements (runtime: {}, compile-time: {})",
               args.size(), max_size());
    for (int_t i = 0; i != max_size(); ++i) {
      data_[i] = ranges::begin(args)[i];
    }
  }

  constexpr auto container() -> data_container& { return data_; }
  constexpr auto container() const -> data_container const& { return data_; }

  constexpr auto data() -> pointer { return data_.data(); }
  constexpr auto data() const -> const_pointer { return data_.data(); }
};

/// Stack bit storage implementation
///
/// \pre MaxRows != dynamic && MaxCols != dynamic
template <template <class> class StorageContainer, int_t NoRows, int_t NoCols,
          int_t MaxRows, int_t MaxCols>
struct storage<bit, StorageContainer, NoRows, NoCols, MaxRows, MaxCols,
               stack_storage> : max_bounds<MaxRows, MaxCols> {
  static_assert(MaxRows != dynamic, "stack storage with dynamic #of rows");
  static_assert(MaxCols != dynamic, "stack storage with dynamic #of columns");
  static_assert(NoRows <= MaxRows, "NoRows <= MaxRows!");
  static_assert(NoCols <= MaxCols, "NoCols <= MaxCols!");

  using max_bounds = max_bounds<MaxRows, MaxCols>;
  using max_bounds::max_no_rows;
  using max_bounds::max_no_cols;
  using max_bounds::max_size;

  using data_container  = std::bitset<max_size()>;
  using reference       = typename data_container::reference;
  using const_reference = const reference;  // note:std::bitset doesn't have a
                                            // const_reference type member

  alignas(32) data_container data_;

  constexpr storage()               = default;
  constexpr storage(storage const&) = default;
  constexpr storage(storage&&)      = default;
  constexpr storage& operator=(storage const&) = default;
  constexpr storage& operator=(storage&&) = default;

  constexpr storage(data_container const& other) : data_(other) {}
  constexpr storage(data_container&& other) : data_(std::move(other)) {}

  constexpr storage(const uint_t no_elements) {
    HM3_MATRIX_ASSERT_NO_ELEMENTS_CONSTRUCTOR;
  }

  constexpr storage(const uint_t no_rows_, const uint_t no_cols_) {
    HM3_MATRIX_ASSERT_NO_ROWS_AND_COLS_CONSTRUCTOR;
  }

  constexpr storage(std::initializer_list<bool> args) {
    HM3_ASSERT(args.size() == max_size(),
               "mismatch #of elements (runtime: {}, compile-time: {})",
               args.size(), max_size());
    for (int_t i = 0; i != max_size(); ++i) {
      data_[i] = ranges::begin(args)[i];
    }
  }

  auto set() -> void { this->data_.set(); }
  auto reset() -> void { this->data_.reset(); }
  auto all() -> bool { return this->data_.all(); }

  constexpr auto container() -> data_container& { return data_; }
  constexpr auto container() const -> data_container const& { return data_; }
};

/// Dynamic bit storage
///
/// \pre MaxRows == dynamic || MaxCols == dynamic
template <template <class> class StorageContainer, int_t NoRows, int_t NoCols,
          int_t MaxRows, int_t MaxCols>
struct storage<bit, StorageContainer, NoRows, NoCols, MaxRows, MaxCols,
               dynamic_storage> : max_bounds<MaxRows, MaxCols> {
  static_assert(MaxRows == dynamic || MaxCols == dynamic,
                "static MaxRows and MaxCols: use stack storage!");

  using data_container
   = boost::dynamic_bitset<unsigned long,
                           Eigen::aligned_allocator<unsigned long>>;
  using reference       = typename data_container::reference;
  using const_reference = typename data_container::const_reference;

  alignas(32) data_container data_;

  using max_bounds = max_bounds<MaxRows, MaxCols>;
  using max_bounds::max_no_rows;
  using max_bounds::max_no_cols;
  using max_bounds::max_size;

  storage()               = default;
  storage(storage const&) = default;
  storage(storage&&)      = default;
  storage& operator=(storage const&) = default;
  storage& operator=(storage&&) = default;

  CONCEPT_REQUIRES(MaxRows == dynamic && MaxCols == 1)
  storage(const uint_t no_elements)
   : max_bounds(no_elements, 1), data_(max_size()) {
    HM3_MATRIX_ASSERT_NO_ELEMENTS_CONSTRUCTOR;
    reset();
  }

  storage(const uint_t no_rows_, const uint_t no_cols_)
   : max_bounds(no_rows_, no_cols_), data_(max_size()) {
    HM3_MATRIX_ASSERT_NO_ROWS_AND_COLS_CONSTRUCTOR;
    reset();
  }

  CONCEPT_REQUIRES(MaxRows == dynamic && MaxCols == 1)
  storage(std::initializer_list<bool> args)
   : max_bounds(args.size(), 1), data_(max_size()) {
    HM3_ASSERT(args.size() == max_size(),
               "mismatch #of elements (runtime: {}, compile-time: {})",
               args.size(), max_size());
    for (int_t i = 0; i != max_size(); ++i) {
      data_[i] = ranges::begin(args)[i];
    }
  }

  auto set() -> void { this->data_.set(); }
  auto reset() -> void { this->data_.reset(); }
  auto all() -> bool { return this->data_.all(); }

  auto find_first() const noexcept { return data_.find_first(); }
  auto find_next(std::size_t pos) const noexcept {
    return data_.find_next(pos);
  }
  auto npos() const noexcept { return data_.npos; }

  template <typename UnaryFunction>
  UnaryFunction&& for_each(UnaryFunction&& uf) {
    auto i = find_first();
    do { uf(i); } while (find_next(i) != npos());
    return std::forward<UnaryFunction>(uf);
  }

  template <typename UnaryFunction>
  UnaryFunction&& for_each(UnaryFunction&& uf) const {
    auto i = find_first();
    do { uf(i); } while (find_next(i) != npos());
    return std::forward<UnaryFunction>(uf);
  }

  constexpr auto container() -> data_container& { return data_; }
  constexpr auto container() const -> data_container const& { return data_; }
};

}  // namespace dense
}  // namespace hm3
