#pragma once
/// \file
///
/// Vector whose elements are allocated within the vector object itself (inline)
#include <hm3/utility/embedded_storage.hpp>
#include <iterator>

namespace hm3 {

namespace fixed_capacity_vector_detail {

template <typename T>  //, CONCEPT_REQUIRES_(not std::is_const<T>{})>
constexpr void swap(T& a, T& b) noexcept {
  uncvref_t<T> tmp = std::move(a);
  a                = std::move(b);
  b                = std::move(tmp);
}

template <typename ForwardIt, CONCEPT_REQUIRES_(ForwardIterator<ForwardIt>{})>
constexpr void slow_rotate(ForwardIt first, ForwardIt n_first, ForwardIt last) {
  using std::swap;
  ForwardIt next = n_first;
  while (first != next) {
    swap(*(first++), *(next++));
    if (next == last) {
      next = n_first;
    } else if (first == n_first) {
      n_first = next;
    }
  }
}

template <typename InputIt, typename OutputIt,
          CONCEPT_REQUIRES_(
           InputIterator<InputIt>{}
           and OutputIterator<OutputIt,
                              decltype(std::move(*std::declval<InputIt>()))>{})>
constexpr OutputIt move(InputIt b, InputIt e, OutputIt to) {
  for (; b != e; ++b, (void)++to) { *to = std::move(*b); }
  return to;
}

template <class BinaryPredicate, class InputIterator1, class InputIterator2>
constexpr bool cmp(InputIterator1 first1, InputIterator1 last1,
                   InputIterator2 first2, InputIterator2 last2,
                   BinaryPredicate pred) {
  for (; first1 != last1 && first2 != last2; ++first1, (void)++first2)
    if (!pred(*first1, *first2)) return false;
  return first1 == last1 && first2 == last2;
}

}  // namespace fixed_capacity_vector_detail

/// Vector with inline storage
///
/// TODO: this is far from good (but it works for me TM)
/// - make it actually zero size for capacity == 0
/// - constexpr constructor/destructor for trivially default
///   constructible and trivially destructible types?
/// - implement the rest of the std::vector interface
/// - make it as exception safe as possible
/// - document undefined behavior
template <typename T, std::size_t Capacity>
struct fixed_capacity_vector : private embedded_storage<T, Capacity> {
 private:
  static_assert(std::is_nothrow_destructible<T>{},
                "inline::vector requires T to be nothrow destructible");
  using base_t = embedded_storage<T, Capacity>;

  using base_t::unsafe_set_size;
  using base_t::unsafe_destroy_all;
  using base_t::unsafe_destroy;

 public:
  using value_type             = typename base_t::value_type;
  using difference_type        = typename base_t::difference_type;
  using reference              = value_type&;
  using const_reference        = value_type const&;
  using pointer                = typename base_t::pointer;
  using const_pointer          = typename base_t::const_pointer;
  using iterator               = typename base_t::pointer;
  using const_iterator         = typename base_t::const_pointer;
  using size_type              = typename base_t::size_type;
  using reverse_iterator       = typename std::reverse_iterator<iterator>;
  using const_reverse_iterator = typename std::reverse_iterator<const_iterator>;

  /// \name Size / capacity
  ///@{
  using base_t::size;
  using base_t::capacity;
  using base_t::empty;
  using base_t::full;

  /// Maximum number of elements that can be allocated in the vector
  static constexpr size_type max_size() noexcept { return capacity(); }

  ///@} // Size / capacity

  /// \name Data access
  ///@{

  using base_t::data;

  ///@} // Data access

  /// \name Iterators
  ///@{

  constexpr iterator begin() noexcept { return data(); }
  constexpr const_iterator begin() const noexcept { return data(); }
  constexpr iterator end() noexcept { return data() + size(); }
  constexpr const_iterator end() const noexcept { return data() + size(); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end() - 1); }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end() - 1);
  }
  reverse_iterator rend() noexcept { return reverse_iterator(data() - 1); }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(data() - 1);
  }

  constexpr const_iterator cbegin() noexcept { return begin(); }
  constexpr const_iterator cbegin() const noexcept { return begin(); }
  constexpr const_iterator cend() noexcept { return end(); }
  constexpr const_iterator cend() const noexcept { return end(); }

  ///@}  // Iterators

 private:
  template <typename It>
  constexpr void assert_iterator_in_range(It it) noexcept {
    if
      constexpr(std::is_pointer<It>{}) {
        HM3_ASSERT(begin() <= it, "iterator not in range");
        HM3_ASSERT(it <= end(), "iterator not in range");
      }
  }

  template <typename It0, typename It1>
  constexpr void assert_valid_iterator_pair(It0 first, It1 last) noexcept {
    if
      constexpr(std::is_pointer<It0>{} and std::is_pointer<It1>{}) {
        HM3_ASSERT(first <= last, "invalid iterator pair");
      }
  }

  template <typename It0, typename It1>
  constexpr void assert_iterator_pair_in_range(It0 first, It1 last) noexcept {
    assert_iterator_in_range(first);
    assert_iterator_in_range(last);
    assert_valid_iterator_pair(first, last);
  }

 public:
  /// \name Element access
  ///
  /// TODO: Enable only for Capacity > 0 ?
  ///@{

  constexpr reference operator[](size_type pos) noexcept {
    HM3_ASSERT(pos < size(), "index {} ouf-of-bounds [0, {})", pos, size());
    return *(begin() + pos);
  };
  constexpr const_reference operator[](size_type pos) const noexcept {
    HM3_ASSERT(pos < size(), "index {} ouf-of-bounds [0, {})", pos, size());
    return *(begin() + pos);
  }

  constexpr reference at(size_type pos) {
    if (pos >= size()) {
      throw std::out_of_range(
       "cannot access fixed_capacity_vector element at pos >= size");
    }
    return (*this)[pos];
  }

  constexpr const_reference at(size_type pos) const {
    if (pos >= size()) {
      throw std::out_of_range(
       "cannot access fixed_capacity_vector element at pos >= size");
    }
    return (*this)[pos];
  }

  constexpr reference front() noexcept {
    HM3_ASSERT(not empty(), "calling front on an empty vector");
    return *begin();
  }
  constexpr const_reference front() const noexcept {
    HM3_ASSERT(not empty(), "calling front on an empty vector");
    return *begin();
  }

  constexpr reference back() noexcept {
    HM3_ASSERT(not empty(), "calling back on an empty vector");
    return *(end() - 1);
  }
  constexpr const_reference back() const noexcept {
    HM3_ASSERT(not empty(), "calling back on an empty vector");
    return *(end() - 1);
  }

  ///@} // Element access

  /// \name Modifiers
  ///@{

  using base_t::pop_back;
  using base_t::emplace_back;

  /// Clears the vector.
  constexpr void clear() noexcept {
    unsafe_destroy_all();
    unsafe_set_size(0);
  }

  /// Appends \p value at the end of the vector.
  template <typename U,
            CONCEPT_REQUIRES_(Constructible<T, U>{}
                              and ranges::Assignable<reference, U&&>{})>
  constexpr void push_back(U&& value) noexcept(
   noexcept(emplace_back(std::forward<U>(value)))) {
    HM3_ASSERT(not full(), "tried push_back on full fixed_capacity_vector!");
    emplace_back(std::forward<U>(value));
  }

  /// Appends a default constructed `T` at the end of the vector.
  CONCEPT_REQUIRES(Constructible<T, T>{}
                   and ranges::Assignable<reference, T&&>{})
  void push_back() noexcept(noexcept(emplace_back(T{}))) {
    HM3_ASSERT(not full(), "tried push_back on full fixed_capacity_vector!");
    emplace_back(T{});
  }

  template <typename... Args, CONCEPT_REQUIRES_(Constructible<T, Args...>{})>
  constexpr iterator emplace(const_iterator position, Args&&... args) noexcept(
   noexcept(move_insert(position, std::declval<value_type*>(),
                        std::declval<value_type*>()))) {
    HM3_ASSERT(not full(), "tried emplace on full fixed_capacity_vector!");
    assert_iterator_in_range(position);
    value_type a(std::forward<Args>(args)...);
    return move_insert(position, &a, &a + 1);
  }
  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr iterator insert(
   const_iterator position,
   const_reference x) noexcept(noexcept(insert(position, size_type(1), x))) {
    HM3_ASSERT(not full(), "tried insert on full fixed_capacity_vector!");
    assert_iterator_in_range(position);
    return insert(position, size_type(1), x);
  }

  CONCEPT_REQUIRES(MoveConstructible<T>{})
  constexpr iterator insert(const_iterator position, value_type&& x) noexcept(
   noexcept(move_insert(position, &x, &x + 1))) {
    HM3_ASSERT(not full(), "tried insert on full fixed_capacity_vector!");
    assert_iterator_in_range(position);
    return move_insert(position, &x, &x + 1);
  }

  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr iterator insert(const_iterator position, size_type n,
                            const T& x) noexcept(noexcept(push_back(x))) {
    assert_iterator_in_range(position);
    const auto new_size = size() + n;
    HM3_ASSERT(new_size <= capacity(), "trying to insert beyond capacity!");
    auto b = end();
    while (n != 0) {
      push_back(x);
      --n;
    }

    auto writable_position = begin() + (position - begin());
    fixed_capacity_vector_detail::slow_rotate(writable_position, b, end());
    return writable_position;
  }

  // template <typename SizeType, CONCEPT_REQUIRES_(Same<SizeType,
  // size_type>{})>
  // constexpr iterator insert(const_iterator position, SizeType n)
  //   noexcept(noexcept(emplace_back()))
  // {
  //   assert_iterator_in_range(position);
  //   const auto new_size = size() + n;
  //   HM3_ASSERT(new_size <= capacity(), "trying to insert beyond capacity!");
  //   auto b = end();
  //   while (n != 0) {
  //     emplace_back();
  //     --n;
  //   }

  //   iterator writable_position = begin() + (position - begin());
  //   fixed_capacity_vector_detail::slow_rotate(writable_position, b, end());
  //   return writable_position;
  // }

  template <class InputIt,
            CONCEPT_REQUIRES_(
             InputIterator<InputIt>{}
             and Constructible<value_type, iterator_reference_t<InputIt>>{})>
  constexpr iterator insert(
   const_iterator position, InputIt first,
   InputIt last) noexcept(noexcept(emplace_back(*first))) {
    assert_iterator_in_range(position);
    assert_valid_iterator_pair(first, last);
    if
      constexpr(RandomAccessIterator<InputIt>{}) {
        HM3_ASSERT(size() + (last - first) <= capacity(),
                   "trying to insert beyond capacity!");
      }
    auto b = end();

    // insert at the end and then just rotate:
    // cannot use try in constexpr function
    // try {  // if copy_constructor throws you get basic-guarantee?
    for (; first != last; ++first) { emplace_back(*first); }
    // } catch (...) {
    //   erase(b, end());
    //   throw;
    // }

    auto writable_position = begin() + (position - begin());
    fixed_capacity_vector_detail::slow_rotate(writable_position, b, end());
    return writable_position;
  }

  template <class InputIt, CONCEPT_REQUIRES_(InputIterator<InputIt>{})>
  constexpr iterator move_insert(
   const_iterator position, InputIt first,
   InputIt last) noexcept(noexcept(emplace_back(std::move(*first)))) {
    assert_iterator_in_range(position);
    assert_valid_iterator_pair(first, last);
    if
      constexpr(RandomAccessIterator<InputIt>{}) {
        HM3_ASSERT(size() + (last - first) <= capacity(),
                   "trying to insert beyond capacity!");
      }
    iterator b = end();
    // cannot use try in constexpr function
    // try {  // if move_constructor throws you get basic-guarantee?
    for (; first != last; ++first) { emplace_back(std::move(*first)); }
    // } catch (...) {
    //   erase(b, end());
    //   throw;
    // }
    // we insert at the end and then just rotate:
    auto writable_position = begin() + (position - begin());
    fixed_capacity_vector_detail::slow_rotate<iterator>(writable_position, b,
                                                        end());
    return writable_position;
  }

  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr iterator insert(
   const_iterator position,
   std::initializer_list<T> il) noexcept(noexcept(insert(position,
                                                         ranges::begin(il),
                                                         ranges::end(il)))) {
    assert_iterator_in_range(position);
    return insert(position, ranges::begin(il), ranges::end(il));
  }

  CONCEPT_REQUIRES(ranges::Movable<reference>{})
  constexpr iterator erase(const_iterator position) noexcept {
    assert_iterator_in_range(position);
    return erase(position, position + 1);
  }

  CONCEPT_REQUIRES(ranges::Movable<value_type>{})
  constexpr iterator erase(const_iterator first, const_iterator last) noexcept {
    assert_iterator_pair_in_range(first, last);
    iterator p = begin() + (first - begin());
    if (first != last) {
      unsafe_destroy(
       fixed_capacity_vector_detail::move(p + (last - first), end(), p), end());
      unsafe_set_size(size() - (last - first));
    }

    return p;
  }

  CONCEPT_REQUIRES(ranges::Assignable<T&, T&&>{})
  constexpr void swap(fixed_capacity_vector& other) noexcept(
   std::is_nothrow_swappable<T>{}) {
    fixed_capacity_vector tmp = std::move(other);
    other                     = std::move(*this);
    (*this)                   = std::move(tmp);
  }

  /// Resizes the container to contain \p sz elements. If elements need to be
  /// appended, these are copy-constructed from \p value.
  ///
  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr void resize(size_type sz, T const& value) noexcept(
   std::is_nothrow_copy_constructible<T>{}
   and std::is_nothrow_destructible<T>{}) {
    if (sz == size()) { return; }
    if (sz > size()) {
      HM3_ASSERT(sz <= capacity(), "fixed_capacity_vector cannot be resized to "
                                   "a size greater than capacity");
      insert(end(), sz - size(), value);
    } else {
      erase(end() - (size() - sz), end());
    }
  }

 private:
  CONCEPT_REQUIRES(MoveConstructible<T>{} or CopyConstructible<T>{})
  constexpr void emplace_n(size_type n) noexcept(
   (MoveConstructible<T>{} and std::is_nothrow_move_constructible<T>{})
   or (CopyConstructible<T>{} and std::is_nothrow_copy_constructible<T>{})) {
    HM3_ASSERT(
     n <= capacity(),
     "fixed_capacity_vector cannot be resized to a size greater than capacity");
    while (n != size()) { emplace_back(T{}); }
  }

 public:
  /// Resizes the container to contain \p sz elements. If elements need to be
  /// appended, these are move-constructed from `T{}` (or copy-constructed if
  /// `T` is not `MoveConstructible`).
  CONCEPT_REQUIRES(ranges::Movable<value_type>{})
  constexpr void resize(size_type sz) noexcept(
   std::is_nothrow_destructible<T>{}
   and ((std::is_move_constructible<T>{}
         and std::is_nothrow_move_constructible<T>{})
        or (std::is_copy_constructible<T>{}
            and std::is_nothrow_copy_constructible<T>{}))) {
    if (sz == size()) { return; }
    if (sz > size()) {
      emplace_n(sz);
    } else {
      erase(end() - (size() - sz), end());
    }
  }

  ///@}  // Modifiers

  /// \name Construct/copy/move/destroy
  ///@{

  /// Default constructor.
  constexpr fixed_capacity_vector() = default;

  /// Copy constructor.
  CONCEPT_REQUIRES(CopyConstructible<value_type>{})
  constexpr fixed_capacity_vector(fixed_capacity_vector const& other) noexcept(
   noexcept(insert(begin(), ranges::begin(other), ranges::end(other)))) {
    // nothin to assert: size of other cannot exceed capacity because both
    // vectors have the same type
    insert(begin(), ranges::begin(other), ranges::end(other));
  }

  /// Move constructor.
  CONCEPT_REQUIRES(MoveConstructible<value_type>{})
  constexpr fixed_capacity_vector(fixed_capacity_vector&& other) noexcept(
   noexcept(move_insert(begin(), ranges::begin(other), ranges::end(other)))) {
    // nothin to assert: size of other cannot exceed capacity because both
    // vectors have the same type
    move_insert(begin(), ranges::begin(other), ranges::end(other));
  }

  /// Copy assignment.
  CONCEPT_REQUIRES(ranges::Assignable<reference, const_reference>{})
  constexpr fixed_capacity_vector&
   operator=(fixed_capacity_vector const& other) noexcept(
    noexcept(clear())
    and noexcept(insert(begin(), ranges::begin(other), ranges::end(other)))) {
    // nothin to assert: size of other cannot exceed capacity because both
    // vectors have the same type
    clear();
    insert(this->begin(), ranges::begin(other), ranges::end(other));
    return *this;
  }

  /// Move assignment.
  CONCEPT_REQUIRES(ranges::Assignable<reference, reference>{})
  constexpr fixed_capacity_vector&
   operator=(fixed_capacity_vector&& other) noexcept(
    noexcept(clear()) and noexcept(move_insert(begin(), ranges::begin(other),
                                               ranges::end(other)))) {
    // nothin to assert: size of other cannot exceed capacity because both
    // vectors have the same type
    clear();
    move_insert(this->begin(), ranges::begin(other), ranges::end(other));
    return *this;
  }

  /// Initializes vector with \p n default-constructed elements.
  CONCEPT_REQUIRES(CopyConstructible<T>{} or MoveConstructible<T>{})
  constexpr fixed_capacity_vector(size_type n) noexcept(
   noexcept(emplace_n(n))) {
    HM3_ASSERT(n <= capacity(), "tried to initialize inline vector of capacity "
                                "{} with {} elements",
               capacity(), n);
    emplace_n(n);
  }

  /// Initializes vector with \p n with \p value.
  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr fixed_capacity_vector(size_type n, T const& value) noexcept(
   noexcept(insert(begin(), n, value))) {
    HM3_ASSERT(n <= capacity(), "tried to initialize inline vector of capacity "
                                "{} with {} elements",
               capacity(), n);
    insert(begin(), n, value);
  }

  /// Initialize vector from range [first, last).
  template <class InputIt, CONCEPT_REQUIRES_(InputIterator<InputIt>{})>
  constexpr fixed_capacity_vector(InputIt first, InputIt last) {
    if
      constexpr(RandomAccessIterator<InputIt>{}) {
        HM3_ASSERT(
         last - first <= capacity(),
         "tried to initialize fixed_capacity_vector with capacity {}, with a "
         "range with {} elements",
         capacity(), last - first);
      }
    insert(begin(), first, last);
  }

  template <typename U, CONCEPT_REQUIRES_(ConvertibleTo<U, value_type>{})>
  constexpr fixed_capacity_vector(std::initializer_list<U> il) noexcept(
   noexcept(base_t(std::move(il))))
   : base_t(std::move(il)) { /* assert happens in base_t constructor */
  }

  template <class InputIt, CONCEPT_REQUIRES_(InputIterator<InputIt>{})>
  constexpr void assign(InputIt first, InputIt last) noexcept(
   noexcept(clear()) and noexcept(insert(begin(), first, last))) {
    if
      constexpr(RandomAccessIterator<InputIt>{}) {
        HM3_ASSERT(
         last - first <= capacity(),
         "tried to initialize fixed_capacity_vector with capacity {}, with a "
         "range with {} elements",
         capacity(), last - first);
      }
    clear();
    insert(begin(), first, last);
  }

  /// An InputRange can be assigned from if its value type is convertible to T.
  template <typename Rng>
  using RangeAssignable
   = meta::strict_and<InputRange<Rng>, ConvertibleTo<range_value_t<Rng>, T>>;

  /// Assign from range \p rng.
  template <typename Rng, CONCEPT_REQUIRES_(RangeAssignable<Rng>{}
                                            and CopyConstructible<T>{})>
  constexpr void range_assign(Rng&& rng)
  // noexcept(noexcept(emplace_back(*begin(rng))))
  {
    if
      constexpr(RandomAccessRange<Rng>{}) {
        HM3_ASSERT(ranges::distance(rng) <= capacity(),
                   "tried to assign range with {} "
                   "elements to fixed_capacity_vector of capacity {}",
                   ranges::distance(rng), capacity());
      }
    for (auto&& i : rng) { emplace_back(std::forward<decltype(i)>(i)); }
  }

  /// Range constructor.
  template <typename Rng, CONCEPT_REQUIRES_(RangeAssignable<Rng>{})>
  constexpr fixed_capacity_vector(Rng&& rng) noexcept(
   noexcept(range_assign(std::forward<Rng>(rng)))) {
    range_assign(std::forward<Rng>(rng));
  }

  /// Construction from Iterator/Sentinel pair.
  template <typename It, typename S,
            CONCEPT_REQUIRES_(InputIterator<It>{} and IteratorRange<It, S>{}
                              and RangeAssignable<iterator_range<It, S>>{})>
  constexpr fixed_capacity_vector(It&& it, S&& s) noexcept(
   noexcept(fixed_capacity_vector(iterator_range<It, S>{std::forward<It>(it),
                                                        std::forward<S>(s)})))
   : fixed_capacity_vector(
      iterator_range<It, S>{std::forward<It>(it), std::forward<S>(s)}) {}

  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr void assign(size_type n, const T& u) {
    HM3_ASSERT(n <= capacity(), "tried to initialize inline vector of capacity "
                                "{} with {} elements",
               capacity(), n);
    clear();
    insert(begin(), n, u);
  }
  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr void assign(std::initializer_list<T> const& il) {
    HM3_ASSERT(il.size() <= capacity(),
               "tried to initialize inline vector of capacity "
               "{} with {} elements",
               capacity(), il.size());
    clear();
    insert(this->begin(), ranges::begin(il), ranges::end(il));
  }
  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr void assign(std::initializer_list<T>&& il) {
    HM3_ASSERT(il.size() <= capacity(),
               "tried to initialize inline vector of capacity "
               "{} with {} elements",
               capacity(), il.size());
    clear();
    insert(this->begin(), ranges::begin(il), ranges::end(il));
  }

  ///@}  // Construct/copy/move/destroy/assign
};

template <typename T, std::size_t Capacity>
constexpr bool operator==(
 fixed_capacity_vector<T, Capacity> const& a,
 fixed_capacity_vector<T, Capacity> const& b) noexcept {
  return a.size() == b.size()
         and fixed_capacity_vector_detail::cmp(a.begin(), a.end(), b.begin(),
                                               b.end(), ranges::equal_to{});
}

template <typename T, std::size_t Capacity>
constexpr bool operator!=(
 fixed_capacity_vector<T, Capacity> const& a,
 fixed_capacity_vector<T, Capacity> const& b) noexcept {
  return not(a == b);
}

}  // namespace hm3
