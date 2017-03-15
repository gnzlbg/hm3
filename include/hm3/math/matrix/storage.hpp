#pragma once
/// \file
///
/// Dense matrix storage
#include <bitset>
#include <hm3/ext/dynamic_bitset.hpp>
#include <hm3/ext/vector.hpp>
#include <hm3/math/matrix/bounds.hpp>
#include <hm3/math/matrix/traits.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/range.hpp>
#define HM3_MATRIX_STORAGE_USE_ALIGNAS

namespace hm3::dense {

#define HM3_MATRIX_ASSERT_NO_ELEMENTS_CONSTRUCTOR \
  HM3_ASSERT(no_elements <= max_size(),           \
             "#of elements: '{}' should be <= "   \
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

  // Alignment requirement, SIMD types generally require alignment to a 16byte
  // boundary. EIGEN_MAKE_ALIGNED_OPERATOR_NEW is used to enforce this
  // requirement for heap allocations.
  static constexpr suint_t alignment() noexcept {
    return max_size() > 1 ? 16 : 0;
  }

  using pointer       = value_type*;
  using const_pointer = value_type const*;

#ifdef HM3_MATRIX_STORAGE_USE_ALIGNAS
  alignas(alignment())  // Required for correctness, since we force EigenMap to
                        // assume proper alignment!
#endif
   data_container data_;

  constexpr storage()               = default;
  constexpr storage(storage const&) = default;

  constexpr storage(storage&&) = default;
  constexpr storage& operator=(storage const&) = default;
  constexpr storage& operator=(storage&&) = default;

  explicit constexpr storage(data_container const& other) : data_(other) {}
  explicit constexpr storage(data_container&& other)
   : data_(std::move(other)) {}
  ~storage() = default;

  explicit constexpr storage(const uint_t no_elements) {
    HM3_MATRIX_ASSERT_NO_ELEMENTS_CONSTRUCTOR;
  }

  constexpr storage(const uint_t no_rows_, const uint_t no_cols_) {
    HM3_MATRIX_ASSERT_NO_ROWS_AND_COLS_CONSTRUCTOR;
  }

 private:
  template <std::size_t... Is>
  constexpr storage(std::index_sequence<Is...>, std::initializer_list<T> v)
   : data_{{ranges::at(v, Is)...}} {}

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

  // #ifdef HM3_MATRIX_STORAGE_USE_ALIGNAS
  //   alignas(32)
  // #endif
  data_container data_;

  using max_bounds = max_bounds<MaxRows, MaxCols>;
  using max_bounds::max_no_rows;
  using max_bounds::max_no_cols;
  using max_bounds::max_size;

  storage()               = default;
  storage(storage const&) = default;
  storage(storage&&)      = default;
  storage& operator=(storage const&) = default;
  storage& operator=(storage&&) = default;
  ~storage()                    = default;

  CONCEPT_REQUIRES(MaxRows == dynamic && MaxCols == 1)
  explicit storage(const uint_t no_elements)
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

 public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#pragma clang diagnostic pop
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

  static constexpr suint_t alignment() noexcept { return 16; }

#ifdef HM3_MATRIX_STORAGE_USE_ALIGNAS
  alignas(alignment())
#endif
   data_container data_;

  constexpr storage()               = default;
  constexpr storage(storage const&) = default;
  constexpr storage(storage&&)      = default;
  constexpr storage& operator=(storage const&) = default;
  constexpr storage& operator=(storage&&) = default;
  ~storage()                              = default;

  explicit constexpr storage(data_container const& other) : data_(other) {}
  explicit constexpr storage(data_container&& other)
   : data_(std::move(other)) {}

  explicit constexpr storage(const uint_t no_elements) {
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

 public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#pragma clang diagnostic pop
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

  using block_type = uint64_t;
  using data_container
   = boost::dynamic_bitset<block_type, Eigen::aligned_allocator<block_type>>;
  using reference       = typename data_container::reference;
  using const_reference = typename data_container::const_reference;

#ifdef HM3_MATRIX_STORAGE_USE_ALIGNAS
  alignas(32)
#endif
   data_container data_;

  using max_bounds = max_bounds<MaxRows, MaxCols>;
  using max_bounds::max_no_rows;
  using max_bounds::max_no_cols;
  using max_bounds::max_size;

  storage()               = default;
  storage(storage const&) = default;
  storage(storage&&)      = default;
  storage& operator=(storage const&) = default;
  storage& operator=(storage&&) = default;
  ~storage()                    = default;

  CONCEPT_REQUIRES(MaxRows == dynamic && MaxCols == 1)
  explicit storage(const uint_t no_elements)
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

 public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#pragma clang diagnostic pop
};

}  // namespace hm3::dense
