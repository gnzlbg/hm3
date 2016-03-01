#pragma once
/// \file
///
/// Vector whose elements are allocated within the vector object itself (inline)
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/range.hpp>
#include <type_traits>

namespace hm3 {

namespace inline_vector_detail {

template <typename T, std::size_t Capacity,
          bool is_trivially_destructible>  //
struct storage {
  static_assert(Capacity != std::size_t{0}, "");
  std::aligned_storage_t<sizeof(T), std::alignment_of<T>::value>
   data_[Capacity];
  std::size_t size_ = 0;

  constexpr const T* data() const noexcept {
    return reinterpret_cast<T const*>(data_);
  }
  constexpr T* data() noexcept { return reinterpret_cast<T*>(data_); }

  constexpr std::size_t size() const noexcept { return size_; }
  constexpr void set_size(std::size_t new_size) noexcept { size_ = new_size; }

  /// Destroys all elements of the vector inplace
  /// (required because of using placement new to construct the elements
  /// on the std::aligned_storage)
  constexpr void destroy_all() noexcept {
    for (auto it = data(), e = data() + size(); it != e; ++it) { it->~T(); }
  }
  constexpr storage()               = default;
  constexpr storage(storage const&) = default;
  constexpr storage& operator=(storage const&) = default;
  constexpr storage(storage&&)                 = default;
  constexpr storage& operator=(storage&&) = default;
  ~storage() { destroy_all(); }
};

template <typename T, std::size_t Capacity>  //
struct storage<T, Capacity, true> {
  static_assert(Capacity != std::size_t{0}, "");
  std::aligned_storage_t<sizeof(T), std::alignment_of<T>::value> data_[Capacity]
   = {};
  std::size_t size_ = 0;

  constexpr const T* data() const noexcept {
    return reinterpret_cast<T const*>(data_);
  }
  constexpr T* data() noexcept { return reinterpret_cast<T*>(data_); }

  constexpr std::size_t size() const noexcept { return size_; }
  constexpr void set_size(std::size_t new_size) noexcept { size_ = new_size; }

  /// For trivially destructible types there is nothing to do
  static constexpr void destroy_all() noexcept {}
};

template <typename T> struct empty_storage {
  constexpr const T* data() const noexcept { return nullptr; }
  constexpr T* data() noexcept { return nullptr; }

  static constexpr std::size_t size() noexcept { return 0; }
  static constexpr void set_size(std::size_t new_size) noexcept {
    HM3_ASSERT(new_size == std::size_t{0}, "");
  }

  /// For trivially destructible types there is nothing to do
  static constexpr void destroy_all() noexcept {}
};

template <typename T>  //
struct storage<T, 0, true> : empty_storage<T> {};

template <typename T>  //
struct storage<T, 0, false> : empty_storage<T> {};

}  // namespace inline_vector_detail

/// Vector with inline storage
///
/// TODO: this is far from good (but it works for me TM)
/// - make it actually zero size for capacity == 0
/// - constexpr constructor/destructor for trivially default
///   constructible and trivially destructible types?
/// - implement the rest of the std::vector interface
/// - make it as exception safe as possible
/// - document undefined behavior
template <typename T, std::size_t Capacity>  //
struct inline_vector
 : inline_vector_detail::storage<T, Capacity,
                                 std::is_trivially_destructible<T>{}> {
 private:
  using base_t
   = inline_vector_detail::storage<T, Capacity,
                                   std::is_trivially_destructible<T>{}>;

 public:
  using base_t::data;
  using base_t::size;

 private:
  using base_t::set_size;
  using base_t::destroy_all;

  static_assert(std::is_nothrow_destructible<T>{},
                "inline::vector requires T to be nothrow destructible");

 public:
  using value_type      = T;
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = T&;
  using const_reference = T const&;
  using pointer         = T*;
  using const_pointer   = T const*;
  using iterator        = T*;
  using const_iterator  = T const*;

  /// Is the vector empty?
  constexpr bool empty() const noexcept { return size() == size_type(0); }

  /// Maximum number of elements that can be allocated in the vector
  ///
  /// Note: just to be compatible with std::vector
  constexpr size_type max_size() const noexcept { return capacity(); }

  /// Maximum number of elements that can be allocated in the vector
  static constexpr size_type capacity() noexcept { return Capacity; }

  /// Begin iterator
  constexpr iterator begin() noexcept { return data(); }
  /// Begin iterator
  constexpr const_iterator begin() const noexcept { return data(); }
  /// Begin const iterator
  constexpr const_iterator cbegin() const noexcept { return begin(); }

  /// End iterator
  constexpr iterator end() noexcept { return data() + size(); }
  /// End iterator
  constexpr const_iterator end() const noexcept { return data() + size(); }
  /// End const iterator
  constexpr const_iterator cend() const noexcept { return end(); }

 private:
  using nothrow_move_constructible = std::is_nothrow_move_constructible<T>;
  using nothrow_copy_constructible = std::is_nothrow_copy_constructible<T>;
  using nothrow_move_assignable    = std::is_nothrow_move_assignable<T>;
  using nothrow_copy_assignable    = std::is_nothrow_copy_assignable<T>;

  /// A range can be assigned from if it's an InputRange and T can either be
  /// MoveConstructed or CopyConstructed from the range's elemenets
  ///
  /// TODO: update this to get the ranges value type, and check that T can be
  /// Move/Copy constructed from the range's elements
  template <typename Rng>
  using RangeAssignable
   = meta::fast_and<InputRange<Rng>,
                    meta::fast_or<MoveConstructible<T>, CopyConstructible<T>>>;

  /// Assign from range: if the range is not an rvalue, always copy assign
  ///
  /// TODO: underconstrained, see RangeAssignable's todo
  template <typename Rng, CONCEPT_REQUIRES_(RangeAssignable<Rng>{}
                                            and CopyConstructible<T>{})>
  constexpr auto range_assign(Rng&& rng) -> std::
   enable_if_t<!std::is_rvalue_reference<decltype(std::forward<Rng>(rng))>{}> {
    for (auto&& i : rng) { emplace_back(i); }
  }
  /// Assign from range: if the range is an rvalue, always move assign
  ///
  /// TODO: underconstrained, see RangeAssignable's todo
  template <typename Rng, CONCEPT_REQUIRES_(RangeAssignable<Rng>{}
                                            and MoveConstructible<T>{})>
  constexpr auto range_assign(Rng&& rng) -> std::
   enable_if_t<std::is_rvalue_reference<decltype(std::forward<Rng>(rng))>{}> {
    for (auto&& i : rng) { emplace_back(std::move(i)); }
  }

 public:
  /// Constructors/Assignment
  ///@{

  constexpr inline_vector() = default;
  constexpr inline_vector(inline_vector const& other) noexcept(
   nothrow_copy_constructible{}) {
    range_assign(other);
  };
  constexpr inline_vector(inline_vector&& other) noexcept(
   nothrow_move_constructible{}) {
    range_assign(std::move(other));
  };
  constexpr inline_vector& operator=(inline_vector const& other) noexcept(
   nothrow_copy_assignable{}) {
    clear();
    range_assign(other);
    return *this;
  }

  constexpr inline_vector& operator=(inline_vector&& other) noexcept(
   nothrow_move_assignable{}) {
    clear();
    range_assign(std::move(other));
    return *this;
  }

  constexpr inline_vector(size_type count) noexcept(
   nothrow_move_constructible{}) {
    while (count != 0) {
      emplace_back(T{});
      --count;
    }
  }

  constexpr inline_vector(size_type count, T const& value) noexcept(
   nothrow_copy_constructible{}) {
    while (count != 0) {
      emplace_back(value);
      --count;
    }
  }

  /// Range constructor
  template <typename Rng, CONCEPT_REQUIRES_(RangeAssignable<Rng>{})>
  constexpr inline_vector(Rng&& rng) {
    range_assign(std::forward<Rng>(rng));
  }

  /// TODO: range assignment

  constexpr inline_vector(std::initializer_list<T> init) {
    range_assign(std::move(init));
  }

  /// Construction from Iterator/Sentinel pair
  template <typename It, typename S,
            CONCEPT_REQUIRES_(InputIterator<It>{} and IteratorRange<It, S>{}
                              and RangeAssignable<iterator_range<It, S>>{})>
  constexpr inline_vector(It&& it, S&& s)
   : inline_vector(
      iterator_range<It, S>{std::forward<It>(it), std::forward<S>(s)}) {}

  ///@}

  /// \name Element access
  ///@{

  constexpr reference operator[](size_type pos) noexcept {
    HM3_ASSERT(pos < size(), "index {} ouf-of-bounds [0,{})", pos, size());
    return *(begin() + pos);
  };
  constexpr const_reference operator[](size_type pos) const noexcept {
    HM3_ASSERT(pos < size(), "index {} ouf-of-bounds [0,{})", pos, size());
    return *(begin() + pos);
  }

  /// TODO: implement throwing at(size_type pos) -> reference
  /// TODO: implement throwing at(size_type pos) const -> const_reference

  constexpr reference front() noexcept {
    HM3_ASSERT(size() > 0, "calling front on an empty vector");
    return *begin();
  }
  constexpr const_reference front() const noexcept {
    HM3_ASSERT(size() > 0, "calling front on an empty vector");
    return *begin();
  }

  constexpr reference back() noexcept {
    HM3_ASSERT(size() > 0, "calling back on an empty vector");
    return *(end() - 1);
  }
  constexpr const_reference back() const noexcept {
    HM3_ASSERT(size() > 0, "calling back on an empty vector");
    return *(end() - 1);
  }

  ///@}

  /// Erase elements in range [first, last)
  iterator erase(const_iterator first, const_iterator last) {
    iterator f = begin() + (first - begin());
    iterator l = begin() + (last - begin());
    if (last != end()) {
      for (iterator i = l + 1, e = end(); i != e; ++i) {
        ranges::swap(*i, *(f++));
      }
    }
    set_size(size() - (end() - f));
    for (; f != l; ++f) { f->~T(); }
    return end();
  }

  /// Clears the vector
  constexpr void clear() noexcept {
    destroy_all();
    set_size(0);
  }

  /// Emplace back
  ///
  /// TODO: placement new is not constexpr so this cannot ever possibly be
  /// constexpr
  template <typename... Args, CONCEPT_REQUIRES_(Constructible<T, Args...>{})>
  void emplace_back(Args&&... args) {
    HM3_ASSERT(size() <= capacity(),
               "cannot emplace back element, vector is full");
    new (const_cast<ranges::uncvref_t<T>*>(data() + size()))
     T(std::forward<Args>(args)...);
    set_size(size() + 1);
  }

  /// Push back
  ///
  /// TODO: Cannot be constexpr because of emplace_back usage of placement new
  template <typename U, CONCEPT_REQUIRES_(Constructible<T, U>{})>
  void push_back(U&& value) {
    emplace_back(std::forward<U>(value));
  }

  /// Pop back
  ///
  /// TODO: Cannot be constexpr because it explicitly calls a destructor
  /// (overload for trivially destructible types and make it constexpr)
  void pop_back() noexcept {
    back()->~T();
    set_size(size() - 1);
  }

  /// Resize
  ///
  /// TODO: Assert size before hand
  /// TODO: cannot be constexpr because of empalce back and erase
  /// TODO: exception safety
  void resize(size_type count, T const& value) {
    if (count > size()) {
      while (count != size()) { emplace_back(value); }
    } else {
      erase(end() - (size() - count), end());
    }
  }

  /// Resize
  ///
  /// TODO: Assert size before hand
  /// TODO: cannot be constexpr because of empalce back and erase
  /// TODO: exception safety
  void resize(size_type count) {
    if (count > size()) {
      while (count != size()) { emplace_back(T{}); }
    } else {
      erase(end() - (size() - count), end());
    }
  }
};

}  // namespace hm3
