#pragma once
/// Small Vector: a vector with the Small Vector Optimizaiton (SVO).
#include <hm3/ext/variant.hpp>
#include <hm3/utility/fixed_capacity_vector.hpp>
#include <hm3/utility/vector.hpp>

namespace hm3 {

template <typename T, std::size_t N, typename A = Eigen::aligned_allocator<T>>
struct small_vector {
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;

  using value_type             = T;
  using reference              = T&;
  using const_reference        = const T&;
  using pointer                = T*;
  using const_pointer          = const T*;
  using iterator               = pointer;
  using const_iterator         = const_pointer;
  using reverse_iterator       = typename std::reverse_iterator<iterator>;
  using const_reverse_iterator = typename std::reverse_iterator<const_iterator>;

  using underlying_t = std::remove_const_t<T>;
  using embedded_t   = fixed_capacity_vector<underlying_t, N>;
  using fallback_t   = std_vector<
   underlying_t,
   typename std::allocator_traits<A>::template rebind_alloc<underlying_t>>;
  using non_const_data_t = variant<embedded_t, fallback_t>;
  using data_t
   = meta::if_c<std::is_const<T>{}, std::add_const_t<non_const_data_t>,
                non_const_data_t>;
  alignas(16) data_t container_;

 private:
  template <typename F>
  constexpr auto match_nc(F&& f)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(visit(f, container_));

  template <typename F>
  constexpr auto match_c(F&& f) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(visit(f, container_));

 public:
  /// \name Size / capacity
  ///@{

  constexpr size_type size() const noexcept {
    return match_c([](auto&& c) -> size_type { return c.size(); });
  }
  constexpr size_type capacity() const noexcept {
    return match_c([](auto&& c) -> size_type { return c.capacity(); });
  }
  constexpr bool empty() const noexcept {
    return match_c([](auto&& c) -> bool { return c.empty(); });
  }

  size_type max_size() const noexcept {
    HM3_ASSERT(fallback_t{}.max_size() >= embedded_t{}.max_size(), "");
    return fallback_t{}.max_size();
  }

  ///@} // Size / capacity

  /// \name Data access
  ///@{

  constexpr pointer data() noexcept {
    return match_nc([](auto&& c) -> pointer { return c.data(); });
  }
  constexpr const_pointer data() const noexcept {
    return match_c([](auto&& c) -> const_pointer { return c.data(); });
  }

  ///@} // Data access

  /// \name Iterators
  ///@{

  constexpr iterator begin() noexcept {
    return match_nc([](auto&& c) -> iterator { return c.data(); });
  }
  constexpr const_iterator begin() const noexcept {
    return match_c([](auto&& c) -> const_iterator { return c.data(); });
  }
  constexpr iterator end() noexcept {
    return match_nc([](auto&& c) -> iterator { return c.data() + c.size(); });
  }
  constexpr const_iterator end() const noexcept {
    return match_c(
     [](auto&& c) -> const_iterator { return c.data() + c.size(); });
  }
  constexpr const_iterator cbegin() noexcept {
    return match_nc([](auto&& c) -> const_iterator { return c.data(); });
  }
  constexpr const_iterator cbegin() const noexcept {
    return match_c([](auto&& c) -> const_iterator { return c.data(); });
  }
  constexpr const_iterator cend() noexcept {
    return match_nc(
     [](auto&& c) -> const_iterator { return c.data() + c.size(); });
  }
  constexpr const_iterator cend() const noexcept {
    return match_c(
     [](auto&& c) -> const_iterator { return c.data() + c.size(); });
  }

  ///@}  // Iterators

  /// \name Element access
  ///
  ///@{

  constexpr reference operator[](size_type pos) noexcept {
    return match_nc([pos](auto&& c) -> reference { return c[pos]; });
  }
  constexpr const_reference operator[](size_type pos) const noexcept {
    return match_c([pos](auto&& c) -> const_reference { return c[pos]; });
  }

  constexpr reference at(size_type pos) {
    return match_nc([pos](auto&& c) -> reference { return c.at(pos); });
  }
  constexpr const_reference at(size_type pos) const {
    return match_c([pos](auto&& c) -> const_reference { return c.at(pos); });
  }

  constexpr reference front() noexcept {
    return match_nc([](auto&& c) -> reference { return c.front(); });
  }
  constexpr const_reference front() const noexcept {
    return match_c([](auto&& c) -> const_reference { return c.front(); });
  }

  constexpr reference back() noexcept {
    return match_nc([](auto&& c) -> reference { return c.back(); });
  }
  constexpr const_reference back() const noexcept {
    return match_c([](auto&& c) -> const_reference { return c.back(); });
  }

  ///@} // Element access

  /// \name Status
  ///@{

  /// Is the small_vector using embedded_storage?
  constexpr bool embedded_storage() const noexcept {
    return get_if<embedded_t>(&container_) != nullptr;
  }

  /// Is the small_vector using fallback_storage?
  constexpr bool fallback_storage() const noexcept {
    return not embedded_storage();
  }

  ///@}  // Status

  /// \name Modifiers
  ///@{

 private:
  /// Grows the vector by `n` elements.
  ///
  /// \pre  size_ = size()
  /// \post size() == size_ + n
  /// \warning Might invalidate _all_ iterators.
  void grow_by(size_type n) {
    // If using embedded storage
    if (embedded_t* ep = get_if<embedded_t>(&container_)) {
      // If the elements fit on embedded storage => nothing to do.
      if (ep->size() + n <= ep->capacity()) { return; }
      // The elements don't fit in the embedded storage:
      fallback_t f;
      f.reserve(ep->size() + n);  // + n => for the elements to be appended
      f.insert(f.end(), std::make_move_iterator(ep->begin()),
               std::make_move_iterator(ep->end()));
      // Switch to fallback storage
      container_ = std::move(f);

      HM3_ASSERT(get_if<embedded_t>(&container_) == nullptr, "");
      HM3_ASSERT(get_if<fallback_t>(&container_) != nullptr, "");
    }
  }

  template <typename C, typename At>
  static constexpr void assert_position_(C const& c, const_iterator p,
                                         At&& at) noexcept {
    HM3_ASSERT_AT(c.begin() <= p and p <= c.end(),
                  "position {} out-of-range [0, {}]", std::forward<At>(at),
                  p - c.begin(), c.size());
  }

  template <typename At>
  constexpr void assert_position(const_iterator p, At&& at) const noexcept {
    assert_position_(*this, p, std::forward<At>(at));
  }

  /// Grows the vector by `n` elements (see `grow_by`) and returns a valid
  /// iterator to the element that `it` pointed to before the growth.
  const_iterator valid_grow_by(size_type n, const_iterator it) noexcept {
    assert_position(it, HM3_AT_);
    auto pos = it - begin();
    grow_by(n);
    return begin() + pos;
  }

  template <typename C>
  static auto as_it(C const& c, const_iterator i) noexcept {
    const auto it_idx = i - c.data();
    HM3_ASSERT(it_idx >= 0 and it_idx <= difference_type(c.size()),
               "it {} out-of-bounds [0, {}]", it_idx, c.size());
    return c.begin()
           + it_idx;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }

  template <typename C, typename It>
  static auto as_ptr(C&& c, It&& it) noexcept {
    const auto it_idx = it - c.begin();
    HM3_ASSERT(it_idx >= 0 and it_idx <= difference_type(c.size()),
               "it {} out-of-bounds [0, {}]", it_idx, c.size());
    return c.data()
           + it_idx;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }

 public:
  template <typename... Args>
  constexpr void push_back(Args&&... args) noexcept {
    grow_by(1);
    match_nc([a_ = std::forward_as_tuple(std::forward<Args>(args)...)](auto&& c)
              ->void {
                std::apply(
                 [&c](auto&&... args) {
                   c.push_back(std::forward<Args>(args)...);
                 },
                 std::move(a_));
              });
  }

  constexpr void pop_back() noexcept {
    // once in fallback_t, pop_back doesn't switch back to embedded_t
    match_nc([](auto&& c) -> void { c.pop_back(); });
  }

  constexpr void clear() noexcept {
    match_nc([](auto&& c) -> void { c.clear(); });
  }

  template <typename... Args>
  constexpr iterator emplace(const_iterator position, Args&&... args) noexcept {
    assert_position(position, HM3_AT_);
    position = valid_grow_by(1, position);
    return match_nc(
     [ a_ = std::forward_as_tuple(std::forward<Args>(args)...),
       &position ](auto&& c)
      ->iterator {
        return std::apply(
         [&c, &position](auto&&... args) -> iterator {
           return as_ptr(
            c, c.emplace(as_it(c, position), std::forward<Args>(args)...));
         },
         std::move(a_));
      });
  }

  template <typename... Args>
  constexpr iterator emplace_back(Args&&... args) noexcept {
    return emplace(end(), std::forward<Args>(args)...);
  }

  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr iterator insert(const_iterator position, const_reference x) {
    assert_position(position, HM3_AT_);
    position = valid_grow_by(1, position);
    return match_nc([&position, &x](auto&& c) -> iterator {
      return as_ptr(c, c.insert(as_it(c, position), x));
    });
  }

  CONCEPT_REQUIRES(MoveConstructible<T>{})
  constexpr iterator insert(const_iterator position, value_type&& x) {
    assert_position(position, HM3_AT_);
    position = valid_grow_by(1, position);
    return match_nc([&position, &x](auto&& c) -> iterator {
      return as_ptr(c, c.insert(as_it(c, position), std::move(x)));
    });
  }

  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr iterator insert(const_iterator position, size_type n, const T& x) {
    assert_position(position, HM3_AT_);
    position = valid_grow_by(1, position);
    return match_nc([&position, n, &x](auto&& c) -> iterator {
      return as_ptr(c, c.insert(as_it(c, position), n, x));
    });
  }

  template <class InputIt,
            CONCEPT_REQUIRES_(
             InputIterator<InputIt>{}
             and Constructible<value_type, iterator_reference_t<InputIt>>{})>
  constexpr iterator insert(const_iterator position, InputIt first,
                            InputIt last) {
    assert_position(position, HM3_AT_);

    // If not using the embedded_storage, just forward to the vector's insert
    // method:
    if (not embedded_storage()) {
      return match_nc([&position, &first, &last](auto&& c) -> iterator {
        return as_ptr(c, c.insert(as_it(c, position), first, last));
      });
    }
    HM3_ASSERT(embedded_storage(), "");

    // If using the embedded storage, and the iterator is a ForwardIterator,
    // precompute the distance and then forward to the underlying container
    // insert method:
    if
      constexpr(ForwardIterator<InputIt>{}) {
        size_type n = ranges::distance(first, last);
        // if the vector switches from embedded to fallback storage, position
        // might point to the wrong memory:
        position = valid_grow_by(n, position);
        return match_nc([&position, &first, &last](auto&& c) -> iterator {
          return as_ptr(c, c.insert(as_it(c, position), first, last));
        });
      }
    else {
      static_assert(InputIterator<InputIt>{}, "");
      // insert returns an iterator to the first element inserted, cache it:
      const auto first_pos_idx = position - begin();
      // If the iterator is an input iterator, we insert the elements one by one
      // until we ran out of embedded storage, and from that point on we forward
      // the call to the fallback implementation:
      auto pos_idx = first_pos_idx;
      while (first != last) {
        HM3_ASSERT(embedded_storage(), "");
        insert(begin() + pos_idx, *first);
        ++pos_idx;
        ++first;
        if (not embedded_storage()) {
          insert(begin() + pos_idx, first, last);
          break;
        }
      }
      // Recompute the position of the first element inserted (in case the
      // vector switched to fallback storage):
      return begin() + first_pos_idx;
    }
    HM3_UNREACHABLE();
  }

  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr iterator insert(
   const_iterator position,
   std::initializer_list<T> il) noexcept(noexcept(insert(position,
                                                         ranges::begin(il),
                                                         ranges::end(il)))) {
    assert_position(position, HM3_AT_);
    return insert(position, ranges::begin(il), ranges::end(il));
  }

  CONCEPT_REQUIRES(ranges::Movable<value_type>{})
  constexpr iterator erase(const_iterator position) noexcept {
    assert_position(position, HM3_AT_);
    return match_nc([&position](auto&& c) -> iterator {
      return as_ptr(c, c.erase(as_it(c, position)));
    });
  }

  CONCEPT_REQUIRES(ranges::Movable<value_type>{})
  constexpr iterator erase(const_iterator first, const_iterator last) noexcept {
    assert_position(first, HM3_AT_);
    assert_position(last, HM3_AT_);
    return match_nc([&first, &last](auto&& c) -> iterator {
      return as_ptr(c, c.erase(as_it(c, first), as_it(c, last)));
    });
  }

  CONCEPT_REQUIRES(ranges::Assignable<T&, T&&>{})
  constexpr void swap(small_vector& other) noexcept(
   std::is_nothrow_swappable<T>{}) {
    small_vector tmp = std::move(other);
    other            = std::move(*this);
    (*this)          = std::move(tmp);
  }

  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr void resize(size_type sz, T const& value) {
    if (embedded_storage()) {
      if (sz <= capacity()) {
        match_nc([&sz, &value](auto&& c) { return c.resize(sz, value); });
        return;
      }
      size_type n = sz - size();
      grow_by(n);
      HM3_ASSERT(not embedded_storage(), "");
      match_nc([&sz, &value](auto&& c) { return c.resize(sz, value); });
      return;
    }
    match_nc([&sz, &value](auto&& c) { return c.resize(sz, value); });
  }

  CONCEPT_REQUIRES(ranges::Movable<value_type>{})
  constexpr void resize(size_type sz) {
    if (embedded_storage()) {
      if (sz <= capacity()) {
        match_nc([&sz](auto&& c) { return c.resize(sz); });
        return;
      }
      size_type n = sz - size();
      grow_by(n);
      HM3_ASSERT(not embedded_storage(), "");
      match_nc([&sz](auto&& c) { return c.resize(sz); });
      return;
    }
    match_nc([&sz](auto&& c) { return c.resize(sz); });
  }

  constexpr void reserve(size_type sz) {
    if (embedded_storage()) {
      if (sz <= capacity()) { return; }
      size_type n = sz - size();
      grow_by(n);
      return;
    }
    if (fallback_t* fp = get_if<fallback_t>(&container_)) {
      fp->reserve(sz);
    } else {
      HM3_ASSERT(false, "");
    }
  }

  ///@} // Modifiers

  /// \name Construct/copy/move/destroy
  ///@{

  /// Default constructor.
  constexpr small_vector() = default;

  /// Copy constructor.
  constexpr small_vector(small_vector const& other) = default;

  /// Move constructor.
  constexpr small_vector(small_vector&& other) = default;

  /// Copy assignment.
  constexpr small_vector& operator=(small_vector const& other) = default;

  /// Move assignment.
  constexpr small_vector& operator=(small_vector&& other) = default;

  /// Destructor
  ~small_vector() = default;

  /// Initializes vector with \p n default-constructed elements.
  CONCEPT_REQUIRES(CopyConstructible<T>{} or MoveConstructible<T>{})
  constexpr small_vector(size_type n) noexcept(noexcept(resize(n))) {
    HM3_ASSERT(n <= capacity(),
               "tried to initialize small vector of capacity "
               "{} with {} elements",
               capacity(), n);
    resize(n);
  }

  /// Initializes vector with \p n with \p value.
  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr small_vector(size_type n,
                         T const& value) noexcept(noexcept(insert(begin(), n,
                                                                  value))) {
    HM3_ASSERT(n <= capacity(),
               "tried to initialize small vector of capacity "
               "{} with {} elements",
               capacity(), n);
    insert(begin(), n, value);
  }

  /// Initialize vector from range [first, last).
  template <class InputIt, CONCEPT_REQUIRES_(InputIterator<InputIt>{})>
  constexpr small_vector(InputIt first, InputIt last) {
    if
      constexpr(RandomAccessIterator<InputIt>{}) {
        HM3_ASSERT(last - first <= difference_type(capacity()),
                   "tried to initialize small_vector with capacity {}, with a "
                   "range with {} elements",
                   capacity(), last - first);
      }
    insert(begin(), first, last);
  }

  template <typename U, CONCEPT_REQUIRES_(ConvertibleTo<U, value_type>{})>
  constexpr small_vector(std::initializer_list<U> il) {
    insert(begin(), ranges::begin(il), ranges::end(il));
  }

  template <class InputIt, CONCEPT_REQUIRES_(InputIterator<InputIt>{})>
  constexpr void assign(InputIt first, InputIt last) noexcept(
   noexcept(clear()) and noexcept(insert(begin(), first, last))) {
    if
      constexpr(RandomAccessIterator<InputIt>{}) {
        HM3_ASSERT(last - first <= capacity(),
                   "tried to initialize small_vector with capacity {}, with a "
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

  template <typename Rng, CONCEPT_REQUIRES_(RangeAssignable<Rng>{})>
  constexpr small_vector(Rng&& rng) {
    insert(begin(), ranges::begin(rng), ranges::end(rng));
  }

  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr void assign(size_type n, const T& u) {
    HM3_ASSERT(n <= capacity(),
               "tried to initialize small vector of capacity "
               "{} with {} elements",
               capacity(), n);
    clear();
    insert(begin(), n, u);
  }
  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr void assign(std::initializer_list<T> const& il) {
    HM3_ASSERT(il.size() <= capacity(),
               "tried to initialize small vector of capacity "
               "{} with {} elements",
               capacity(), il.size());
    clear();
    insert(this->begin(), ranges::begin(il), ranges::end(il));
  }
  CONCEPT_REQUIRES(CopyConstructible<T>{})
  constexpr void assign(std::initializer_list<T>&& il) {
    HM3_ASSERT(il.size() <= capacity(),
               "tried to initialize small vector of capacity "
               "{} with {} elements",
               capacity(), il.size());
    clear();
    insert(this->begin(), ranges::begin(il), ranges::end(il));
  }

  ///@}  // Construct/copy/move/destroy/assign

  friend bool operator==(small_vector<T, N, A> const& a,
                         small_vector<T, N, A> const& b) noexcept {
    return a.size() == b.size()
           and ranges::equal(a.begin(), a.end(), b.begin(), b.end());
  }
};

template <typename T, std::size_t N, typename A>
constexpr bool operator!=(small_vector<T, N, A> const& a,
                          small_vector<T, N, A> const& b) noexcept {
  return !(a == b);
}

}  // namespace hm3
