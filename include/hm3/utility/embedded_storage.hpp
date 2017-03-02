#pragma once
/// \file
///
/// Embedded storage.
#include <cstdint>
#include <hm3/utility/array.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/config/fatal_error.hpp>
#include <hm3/utility/range.hpp>
#include <limits>
#include <type_traits>

/// Forces embedded_storage to align its contents at list to the SIMD alignment
/// boundary.
#define HM3_EMBEDDED_STORAGE_SIMD_ALIGNMENT

namespace hm3 {

namespace embedded_storage_detail {

template <std::size_t N>
using smallest_size_t = std::conditional_t<
 (N < std::numeric_limits<uint8_t>::max()), std::uint8_t,
 std::conditional_t<
  (N < std::numeric_limits<uint16_t>::max()), std::uint16_t,
  std::conditional_t<
   (N < std::numeric_limits<uint32_t>::max()), std::uint32_t,
   std::conditional_t<(N < std::numeric_limits<uint64_t>::max()), std::uint64_t,
                      std::size_t>>>>;

/// Alignment requirements of type `T`.
template <typename T>
static constexpr std::size_t alignment() noexcept {
#ifdef HM3_EMBEDDED_STORAGE_SIMD_ALIGNMENT
  return alignof(T) > 16 ? alignof(T) : 16;
#else
  return alignof(T);
#endif
}

/// Storage for zero elements
template <typename T>
struct empty_storage {
  using size_type       = std::uint8_t;
  using value_type      = T;
  using difference_type = std::ptrdiff_t;
  using pointer         = T*;
  using const_pointer   = T const*;

  static constexpr T* data() noexcept { return nullptr; }
  static constexpr size_type size() noexcept { return 0; }
  static constexpr size_type capacity() noexcept { return 0; }
  static constexpr bool empty() noexcept { return true; }
  static constexpr bool full() noexcept { return true; }

  template <typename... Args, CONCEPT_REQUIRES_(Constructible<T, Args...>{})>
  static constexpr void emplace_back(Args&&...) noexcept {
    HM3_FATAL_ERROR("tried to emplace_back on empty storage");
  }
  static constexpr void pop_back() noexcept {
    HM3_FATAL_ERROR("tried to pop_back on empty storage");
  }
  static constexpr void unsafe_set_size(size_type new_size) noexcept {
    HM3_ASSERT(new_size == 0, "tried to change size of empty storage to {}",
               new_size);
  }

  template <typename InputIt, CONCEPT_REQUIRES_(InputIterator<InputIt>{})>
  static constexpr void unsafe_destroy(InputIt, InputIt) noexcept {}

  /// Destroys all elements of the vector without changing its size (unsafe)
  static constexpr void unsafe_destroy_all() noexcept {}

  constexpr empty_storage()                     = default;
  constexpr empty_storage(empty_storage const&) = default;
  constexpr empty_storage& operator=(empty_storage const&) = default;
  constexpr empty_storage(empty_storage&&)                 = default;
  constexpr empty_storage& operator=(empty_storage&&) = default;

  template <typename U, CONCEPT_REQUIRES_(ConvertibleTo<U, T>{})>
  constexpr empty_storage(std::initializer_list<U> il) noexcept {
    HM3_ASSERT(
     il.size() == 0,
     "tried to construct empty_storage from a non-empty initializer list");
  }
};

/// Storage for trivial elements
template <typename T, std::size_t Capacity>  //
struct trivial_storage {
  static_assert(std::is_trivial<T>{}, "Requires Trivial<T>");
  static_assert(Capacity != std::size_t{0},
                "Capacity must be greater than zero!");

  /// Smallest size_type that can represent Capacity:
  using size_type       = smallest_size_t<Capacity>;
  using value_type      = T;
  using difference_type = std::ptrdiff_t;
  using pointer         = T*;
  using const_pointer   = T const*;

 private:
  /// Number of elements allocated in the embedded storage:
  size_type size_ = 0;

  // If the value_type is const, make a const array of non-const elements:
  using data_t = meta::if_c<not std::is_const<T>{}, array<T, Capacity>,
                            const array<std::remove_const_t<T>, Capacity>>;
  alignas(alignment<T>()) data_t data_{};

 public:
  /// Direct access to the underlying storage.
  ///
  /// Complexity: O(1) in time and space.
  constexpr const T* data() const noexcept { return data_.data(); }

  /// Direct access to the underlying storage.
  ///
  /// Complexity: O(1) in time and space.
  constexpr T* data() noexcept { return data_.data(); }

  /// Number of elements in the storage.
  ///
  /// Complexity: O(1) in time and space.
  constexpr size_type size() const noexcept { return size_; }

  /// Maximum number of elements that can be allocated in the storage.
  ///
  /// Complexity: O(1) in time and space.
  static constexpr size_type capacity() noexcept { return Capacity; }

  /// Is the storage empty?
  constexpr bool empty() const noexcept { return size() == size_type{0}; }

  /// Is the storage full?
  constexpr bool full() const noexcept { return size() == Capacity; }

  /// Constructs an element in-place at the end of the embedded storage.
  ///
  /// Complexity: O(1) in time and space.
  /// Contract: the storage is not full.
  template <typename... Args,
            CONCEPT_REQUIRES_(Constructible<T, Args...>{}
                              and ranges::Assignable<value_type&, T>{})>
  constexpr void emplace_back(Args&&... args) noexcept {
    HM3_ASSERT(not full(), "tried to emplace_back on full storage!");
    data_[size()] = T(std::forward<Args>(args)...);
    unsafe_set_size(size() + 1);
  }

  /// Remove the last element from the container.
  ///
  /// Complexity: O(1) in time and space.
  /// Contract: the storage is not empty.
  constexpr void pop_back() noexcept {
    HM3_ASSERT(not empty(), "tried to pop_back from empty storage!");
    unsafe_set_size(size() - 1);
  }

  /// (unsafe) Changes the container size to \p new_size.
  ///
  /// Contract: `new_size <= capacity()`.
  /// \warning No elements are constructed or destroyed.
  constexpr void unsafe_set_size(size_type new_size) noexcept {
    HM3_ASSERT(new_size <= Capacity, "new_size: {} out-of-bounds [0, {})]",
               new_size, Capacity);
    size_ = new_size;
  }

  /// (unsafe) Destroy elements in the range [begin, end).
  ///
  /// \warning: The size of the storage is not changed.
  template <typename InputIt, CONCEPT_REQUIRES_(InputIterator<InputIt>{})>
  constexpr void unsafe_destroy(InputIt, InputIt) noexcept {}

  /// (unsafe) Destroys all elements of the storage.
  ///
  /// \warning: The size of the storage is not changed.
  static constexpr void unsafe_destroy_all() noexcept {}

  constexpr trivial_storage() noexcept                       = default;
  constexpr trivial_storage(trivial_storage const&) noexcept = default;
  constexpr trivial_storage& operator=(trivial_storage const&) noexcept
   = default;
  constexpr trivial_storage(trivial_storage&&) noexcept = default;
  constexpr trivial_storage& operator=(trivial_storage&&) noexcept = default;

 private:
  template <typename U, CONCEPT_REQUIRES_(ConvertibleTo<U, T>{})>
  static constexpr data_t unsafe_recast_init_list(
   std::initializer_list<U>& il) noexcept {
    HM3_ASSERT(il.size() <= capacity(),
               "trying to construct storage from an initializer_list whose "
               "size (= {}) exceed the storage capacity (= {}})",
               il.size(), capacity());
    std::remove_const_t<data_t> d_{};
    for (std::size_t i = 0, e = il.size(); i < e; ++i) {
      d_[i] = il.begin()[i];
    }
    return d_;
  }

 public:
  /// Constructor from initializer list.
  ///
  /// Contract: `il.size() <= capacity()`.
  template <typename U, CONCEPT_REQUIRES_(ConvertibleTo<U, T>{})>
  constexpr trivial_storage(std::initializer_list<U> il) noexcept
   : data_(unsafe_recast_init_list(il)) {
    unsafe_set_size(il.size());
  }
};

/// Storage for non-trivial elements
template <typename T, std::size_t Capacity>  //
struct non_trivial_storage {
  static_assert(not std::is_trivial<T>{}, "Requires !Trivial<T>");
  static_assert(Capacity != std::size_t{0},
                "Capacity must be greater than zero!");

  /// Smallest size_type that can represent Capacity:
  using size_type       = smallest_size_t<Capacity>;
  using value_type      = T;
  using difference_type = std::ptrdiff_t;
  using pointer         = T*;
  using const_pointer   = T const*;

 private:
  /// Number of elements allocated in the embedded storage:
  size_type size_ = 0;

  using aligned_storage_t
   = std::aligned_storage_t<sizeof(std::remove_const_t<T>),
                            alignof(std::remove_const_t<T>)>;
  using data_t = meta::if_c<not std::is_const<T>{}, aligned_storage_t,
                            const aligned_storage_t>;
  alignas(alignment<T>()) data_t data_[Capacity]{};

 public:
  /// Direct access to the underlying storage.
  ///
  /// Complexity: O(1) in time and space.
  constexpr const T* data() const noexcept {
    return reinterpret_cast<const T*>(data_);
  }

  /// Direct access to the underlying storage.
  ///
  /// Complexity: O(1) in time and space.
  constexpr T* data() noexcept { return reinterpret_cast<T*>(data_); }

  /// Number of elements in the storage.
  ///
  /// Complexity: O(1) in time and space.
  constexpr size_type size() const noexcept { return size_; }

  /// Maximum number of elements that can be allocated in the storage.
  ///
  /// Complexity: O(1) in time and space.
  static constexpr size_type capacity() noexcept { return Capacity; }

  /// Is the storage empty?
  constexpr bool empty() const noexcept { return size() == size_type{0}; }

  /// Is the storage full?
  constexpr bool full() const noexcept { return size() == Capacity; }

  /// Constructs an element in-place at the end of the embedded storage.
  ///
  /// Complexity: O(1) in time and space.
  /// Contract: the storage is not full.
  template <typename... Args, CONCEPT_REQUIRES_(Constructible<T, Args...>{})>
  void emplace_back(Args&&... args) noexcept(
   noexcept(new (data() + size()) T(std::forward<Args>(args)...))) {
    HM3_ASSERT(not full(), "tried to emplace_back on full storage");
    new (data() + size()) T(std::forward<Args>(args)...);
    unsafe_set_size(size() + 1);
  }

  /// Remove the last element from the container.
  ///
  /// Complexity: O(1) in time and space.
  /// Contract: the storage is not empty.
  void pop_back() noexcept(std::is_nothrow_destructible<T>{}) {
    HM3_ASSERT(not empty(), "tried to pop_back from empty storage!");
    auto ptr = data() + size() - 1;
    ptr->~T();
    unsafe_set_size(size() - 1);
  }

  /// (unsafe) Changes the container size to \p new_size.
  ///
  /// Contract: `new_size <= capacity()`.
  /// \warning No elements are constructed or destroyed.
  constexpr void unsafe_set_size(size_type new_size) noexcept {
    HM3_ASSERT(new_size <= Capacity, "new_size: {} out-of-bounds [0, {})]",
               new_size, Capacity);
    size_ = new_size;
  }

  /// (unsafe) Destroy elements in the range [begin, end).
  ///
  /// \warning: The size of the storage is not changed.
  template <typename InputIt, CONCEPT_REQUIRES_(InputIterator<InputIt>{})>
  constexpr void unsafe_destroy(InputIt first, InputIt last) noexcept(
   std::is_nothrow_destructible<T>{}) {
    HM3_ASSERT(first >= data() and first <= data() + size(),
               "first is out-of-bounds");
    HM3_ASSERT(last >= data() and last <= data() + size(),
               "last is out-of-bounds");
    for (; first != last; ++first) { first->~T(); }
  }

  /// (unsafe) Destroys all elements of the storage.
  ///
  /// \warning: The size of the storage is not changed.
  constexpr void unsafe_destroy_all() noexcept(
   std::is_nothrow_destructible<T>{}) {
    unsafe_destroy(data(), data() + size());
  }

  constexpr non_trivial_storage()                           = default;
  constexpr non_trivial_storage(non_trivial_storage const&) = default;
  constexpr non_trivial_storage& operator=(non_trivial_storage const&)
   = default;
  constexpr non_trivial_storage(non_trivial_storage&&) = default;
  constexpr non_trivial_storage& operator=(non_trivial_storage&&) = default;

  /// Constructor from initializer list.
  ///
  /// Contract: `il.size() <= capacity()`.
  template <typename U, CONCEPT_REQUIRES_(ConvertibleTo<U, T>{})>
  constexpr non_trivial_storage(std::initializer_list<U> il) noexcept(
   noexcept(emplace_back(il.begin()[0]))) {
    HM3_ASSERT(il.size() <= capacity(),
               "trying to construct storage from an initializer_list whose "
               "size (= {}) exceed the storage capacity (= {}})",
               il.size(), capacity());
    for (std::size_t i = 0; i < il.size(); ++i) { emplace_back(il.begin()[i]); }
  }
};

template <typename T, std::size_t Capacity>
using embedded_storage
 = meta::if_c<Capacity == 0, empty_storage<T>,
              meta::if_<std::is_trivial<T>, trivial_storage<T, Capacity>,
                        non_trivial_storage<T, Capacity>>>;

}  // namespace embedded_storage_detail

using embedded_storage_detail::embedded_storage;

}  // namespace hm3
