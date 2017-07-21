#pragma once
/// \file
///
/// A flat set implemented as a vector-like container adaptor.
#include <hm3/utility/range.hpp>
#include <hm3/utility/small_vector.hpp>
#include <hm3/utility/vector.hpp>

namespace hm3 {

namespace flat_set_detail {
template <typename K, template <typename> typename Vector>
using storage_t = meta::if_c<std::is_const_v<K>,
                             const Vector<std::remove_const_t<K>>, Vector<K>>;
}  // namespace flat_set_detail

struct sorted_uniqued_t {};
static constexpr inline sorted_uniqued_t sorted_uniqued{};

/// A set that models ContiguousContainer.
///
/// \tparam K key-type stored in the container.
/// \tparam Compare BinaryPredicate + Strict Weak Ordering Relation.
/// \tparam Vector vector-like container used as storage.
template <typename K, typename Compare = std::less<K>,
          template <typename> typename Vector = vector>
struct flat_set : private flat_set_detail::storage_t<K, Vector>, Compare {
  using storage_t = flat_set_detail::storage_t<K, Vector>;
  static_assert(ranges::Relation<Compare, K, K>{});
  static_assert(ranges::Predicate<Compare, K, K>{});

  using const_key = meta::_t<std::is_const<K>>;

  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;

  using value_type             = K;
  using const_reference        = typename storage_t::const_reference&;
  using reference              = const_reference;
  using const_pointer          = typename storage_t::const_pointer;
  using pointer                = const_pointer;
  using const_iterator         = typename storage_t::const_iterator;
  using iterator               = const_iterator;
  using const_reverse_iterator = typename std::reverse_iterator<const_iterator>;
  using reverse_iterator       = const_reverse_iterator;

  using storage_t::size;
  using storage_t::capacity;
  using storage_t::empty;
  using storage_t::max_size;

  /// \name Immutable data access
  ///@{

  /// Pointer to the first element of the container.
  constexpr const_pointer data() const noexcept { return get_vector().data(); }

  /// Iterator to the beginning of the container.
  constexpr const_iterator begin() const noexcept {
    return get_vector().begin();
  }
  /// Iterator to one past the last element of the container..
  constexpr const_iterator end() const noexcept { return get_vector().end(); }
  /// Iterator to the beginning of the container.
  constexpr const_iterator cbegin() const noexcept {
    return get_vector().cbegin();
  }
  /// Iterator to one past the last element of the container..
  constexpr const_iterator cend() const noexcept { return get_vector().cend(); }

  /// Unchecked reference to the key at position \p pos (nth_element).
  constexpr const_reference operator[](size_type pos) const noexcept {
    HM3_ASSERT(pos < size(), "position {} out-of-bounds [0, {})", pos, size());
    return get_vector()[pos];
  }
  /// Checked reference to the key at position \p pos (nth_element). Throws if
  /// the key does not exist.
  constexpr const_reference at(size_type pos) const {
    return get_vector().at(pos);
  }
  /// Unchecked reference to the first element of the container.
  constexpr const_reference front() const noexcept {
    return get_vector().front();
  }
  /// Unchecked reference to the last element of the container.
  constexpr const_reference back() const noexcept {
    return get_vector().back();
  }

  ///@}  // Immutable data access

 private:
  constexpr storage_t& get_vector() noexcept {
    return static_cast<storage_t&>(*this);
  }
  constexpr storage_t const& get_vector() const noexcept {
    return static_cast<storage_t const&>(*this);
  }
  constexpr Compare& get_compare() noexcept {
    return static_cast<Compare&>(*this);
  }
  constexpr Compare const& get_compare() const noexcept {
    return static_cast<Compare const&>(*this);
  }

  /// Sorts and uniques the elements of the vector.
  constexpr void sort_unique() noexcept {
    ranges::sort(get_vector(), get_compare());
    get_vector().erase(ranges::unique(get_vector()), get_vector().end());
  }

 public:
  /// \name Constructors / Assignment
  ///@{

  constexpr flat_set(const Compare& comp = Compare(),
                     storage_t&& vec     = storage_t())
   : storage_t(std::move(vec)), Compare(comp) {
    sort_unique();
  }

  constexpr flat_set(sorted_uniqued_t, const Compare& comp = Compare(),
                     storage_t&& vec = storage_t())
   : storage_t(std::move(vec)), Compare(comp) {
    HM3_ASSERT(ranges::is_sorted(get_vector(), comp), "Input not sorted!\n{}",
               get_vector());
  }

  constexpr flat_set(const flat_set& other) = default;
  constexpr flat_set(flat_set&& other)      = default;
  constexpr flat_set& operator=(flat_set const& other) = default;
  constexpr flat_set& operator=(flat_set&& other) = default;

  explicit constexpr flat_set(storage_t&& vec)
   : flat_set(Compare(), std::move(vec)) {}

  explicit constexpr flat_set(sorted_uniqued_t, storage_t&& vec)
   : flat_set(sorted_uniqued, Compare(), std::move(vec)) {}

  template <class InputIt>
  constexpr flat_set(InputIt first, InputIt last,
                     const Compare& comp = Compare())
   : storage_t(first, last), Compare(comp) {
    static_assert(InputIterator<InputIt>{});
    static_assert(ConvertibleTo<ranges::iterator_value_t<InputIt>, K>{});
    sort_unique();
  }

  CONCEPT_REQUIRES(not const_key())
  constexpr flat_set(std::initializer_list<value_type> il,
                     const Compare& comp = Compare())
   : flat_set(ranges::begin(il), ranges::end(il), comp) {}

  CONCEPT_REQUIRES(const_key())
  constexpr flat_set(std::initializer_list<value_type> il,
                     const Compare& comp = Compare())
   : flat_set(sorted_uniqued, comp,
              std::remove_const_t<storage_t>(ranges::begin(il), ranges::end(il))
               | action::sort(comp) | action::unique) {}

 private:
  template <typename Rng>
  using insertable_range
   = meta::if_<meta::not_<Same<uncvref_t<Rng>, storage_t>>,
               meta::not_<Same<uncvref_t<Rng>, value_type>>,
               ConvertibleTo<ranges::range_reference_t<Rng>, value_type>>;

 public:
  template <typename Rng,
            CONCEPT_REQUIRES_(
             insertable_range<Rng>{}
             and not Same<uncvref_t<Rng>, std::initializer_list<value_type>>{}
             and not const_key{})>
  explicit constexpr flat_set(Rng&& rng, const Compare& comp = Compare())
   : flat_set(ranges::begin(rng), ranges::end(rng), comp) {}

  template <typename Rng,
            CONCEPT_REQUIRES_(
             insertable_range<Rng>{}
             and not Same<uncvref_t<Rng>, std::initializer_list<value_type>>{}
             and const_key{})>
  explicit constexpr flat_set(Rng&& rng, const Compare& comp = Compare())
   : flat_set(
      comp, std::remove_const_t<storage_t>(ranges::begin(rng), ranges::end(rng))
             | action::sort(comp) | action::unique) {}

  ///@}  Constructors / Assignment

  /// \name Mutating operations
  ///@{

  using storage_t::clear;

  /// Inserts the key \p v in the set. Returns a pair of an iterator to the
  /// inserted value and a boolean which is true if an insertion happenend, and
  /// false otherwise.
  template <typename K_, CONCEPT_REQUIRES_(ConvertibleTo<K_, value_type>{})>
  constexpr pair<const_iterator, bool> insert(K_&& v) {
    auto pos = lower_bound(v);
    if (pos != end() and *pos == v) { return {pos, false}; }
    get_vector().insert(pos, std::forward<K_>(v));
    return {pos, true};
  }

  /// Inserts the range [first, last) into the set.
  template <typename InputIt>
  constexpr void insert(InputIt first, InputIt last) {
    // cannot assume that the range first last is sorted, or that it does not
    // contain duplicate elements :/
    get_vector().insert(end(), first, last);
    sort_unique();
  }

  /// Inserts the range \p range into the set.
  template <typename Rng, CONCEPT_REQUIRES_(insertable_range<Rng>{})>
  constexpr void insert(Rng&& rng) {
    insert(ranges::begin(rng), ranges::end(rng));
  }

  /// Inserts initializer list \p rng.
  constexpr void insert(std::initializer_list<value_type> rng) {
    insert(ranges::begin(rng), ranges::end(rng));
  }

  // note: emplace does not really make sense, because we need to construct the
  // value anyways before inserting in the container to look up if it exists.
  // (it make sense for node-based containers though, because you can emplace
  // inside the node, but a flat set is not node-based).

  /// Erases the key at position \p it.
  constexpr const_iterator erase(const_iterator it) noexcept {
    HM3_ASSERT(begin() <= it and it <= end(), "iterator out-of-bounds");
    return get_vector().erase(it);
  }

  /// Erases the keys in range [first, last).
  constexpr const_iterator erase(const_iterator first,
                                 const_iterator last) noexcept {
    HM3_ASSERT(begin() <= first and first <= end(), "iterator out-of-bounds");
    HM3_ASSERT(begin() <= last and last <= end(), "iterator out-of-bounds");
    return get_vector().erase(first, last);
  }

  /// Erases the key that compares equal to \p v. Returns the number of keys
  /// erased (0 or 1).
  template <typename K_,
            CONCEPT_REQUIRES_(not Same<uncvref_t<K_>, const_iterator>{})>
  constexpr std::size_t erase(K_&& v) noexcept {
    auto pos = lower_bound(std::forward<K_>(v));
    if (pos != end() and *pos == v) {
      erase(pos);
      return 1;
    }
    return 0;
  }

  /// Replaces the key that compares equal to \p v with \p v.
  template <typename K_>
  constexpr void replace(K_&& v) noexcept {
    auto pos = lower_bound(v);
    HM3_ASSERT(pos != end() and *pos == v,
               "cannot replace a non-existing element");
    if (pos != end() and *pos == v) {
      (*const_cast<pointer>(pos)) = std::forward<K_>(v);
    }
  }

  // swap (use default)

  ///@} // Mutating operations

  /// \name Immutable operations
  ///@{

  /// Iterator to the largest element smaller than \p k.
  template <typename K_>
  constexpr const_iterator lower_bound(K_&& k) const noexcept {
    return ranges::lower_bound(get_vector(), std::forward<K_>(k),
                               get_compare());
  }

  /// Iterator to the smallest element larger than \p k.
  template <typename K_>
  constexpr const_iterator upper_bound(K_&& k) const noexcept {
    return ranges::upper_bound(get_vector(), std::forward<K_>(k),
                               get_compare());
  }

  /// Pair of iterators (lower_bound(k), upper_bound(k)).
  template <typename K_>
  constexpr pair<const_iterator, const_iterator> equal_range(K_&& k) const
   noexcept {
    return ranges::equal_range(get_vector(), std::forward<K_>(k),
                               get_compare());
  }

  /// Iterator to the element that compares equal to \p k.
  template <typename K_>
  constexpr const_iterator find(K_&& k) const noexcept {
    auto it = lower_bound(std::forward<K_>(k));
    if (it == end() or *it == k) { return it; }
    return end();
  }

  /// Number of elements that compares equal to \p k (0 or 1).
  template <typename K_>
  constexpr std::size_t count(K_&& k) const noexcept {
    return find(std::forward<K_>(k)) == end() ? 0 : 1;
  }

  /// Does the set contain an element that compares equal to \p k?
  template <typename K_>
  constexpr bool contains(K_&& k) const noexcept {
    return find(std::forward<K_>(k)) == end() ? false : true;
  }

  ///@}  // Immutable operations

  /// Releases the vector.
  storage_t release() noexcept { return std::move(get_vector()); }

  /// Returns a copy of the object that compares the keys.
  constexpr Compare key_comp() const noexcept { return get_compare(); }

  /// Swaps two flat_sets.
  void swap(flat_set& other) noexcept {
    ranges::swap(get_vector(), other.get_vector());
  }

  /// \name Lexicographical comparison operators.
  ///@{

  friend constexpr bool operator==(flat_set const& a,
                                   flat_set const& b) noexcept {
    return a.get_vector() == b.get_vector();
  }

  friend constexpr bool operator!=(flat_set const& a,
                                   flat_set const& b) noexcept {
    return a.get_vector() != b.get_vector();
  }

  friend constexpr bool operator<(flat_set const& a,
                                  flat_set const& b) noexcept {
    return a.get_vector() < b.get_vector();
  }

  friend constexpr bool operator<=(flat_set const& a,
                                   flat_set const& b) noexcept {
    return a.get_vector() <= b.get_vector();
  }

  friend constexpr bool operator>(flat_set const& a,
                                  flat_set const& b) noexcept {
    return a.get_vector() > b.get_vector();
  }

  friend constexpr bool operator>=(flat_set const& a,
                                   flat_set const& b) noexcept {
    return a.get_vector() >= b.get_vector();
  }

  ///@}  // Comparison operators

 private:
  template <typename T>
  struct unit {
    auto operator()() const noexcept { return T{}; }
  };

 public:
  /// Monadic map
  template <typename K_, typename ThenF,
            typename ElseF
            = unit<decltype(std::declval<ThenF>(std::declval<K_>()))>>
  auto map(K_&& key, ThenF&& tf, ElseF&& ef = ElseF{}) {
    if (auto it = find(key); it != end()) {
      return tf(*it);
    } else {
      return ef();
    }
  }
};

namespace flat_set_detail {

template <std::size_t N>
struct small_vector_ {
  template <typename K>
  using type = small_vector<K, N>;
};

}  // namespace flat_set_detail

template <typename K, std::size_t N, typename C = std::less<K>>
using small_flat_set
 = flat_set<K, C, flat_set_detail::small_vector_<N>::template type>;

}  // namespace hm3
