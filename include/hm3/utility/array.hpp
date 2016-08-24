#pragma once
/// \file
///
/// Array
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>
#include <iterator>
#include <utility>

namespace hm3 {

/// A constexpr-friendly implementation of std::array
template <typename T, suint_t N>
struct array {  // NOLINT
  using self                   = array;
  using value_type             = T;
  using reference              = value_type&;
  using const_reference        = value_type const&;
  using iterator               = value_type*;
  using const_iterator         = value_type const*;
  using pointer                = value_type*;
  using const_pointer          = value_type const*;
  using size_type              = suint_t;
  using difference_type        = std::ptrdiff_t;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  value_type elems_[N > 0 ? N : 1];  // TODO: use std::conditional_t

  // No explicit construct/copy/destroy for aggregate type
  constexpr array() = default;
  constexpr void fill(const value_type& u) {
    for (suint_t i = 0; i < N; ++i) { elems_[i] = u; }
  }
  constexpr void swap(array& a) noexcept(
   ranges::is_nothrow_swappable<T, T>::value) {
    using std::swap;
    for (suint_t i = 0; i < N; ++i) { swap(elems_[i], a.elems_[i]); }
  }
  // iterators:
  constexpr iterator begin() noexcept { return iterator(elems_); }
  constexpr const_iterator begin() const noexcept {
    return const_iterator(elems_);
  }
  constexpr iterator end() noexcept { return iterator(elems_ + N); }
  constexpr const_iterator end() const noexcept {
    return const_iterator(elems_ + N);
  }
  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }
  constexpr const_iterator cbegin() const noexcept { return begin(); }
  constexpr const_iterator cend() const noexcept { return end(); }
  constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  constexpr const_reverse_iterator crend() const noexcept { return rend(); }
  // capacity:
  static constexpr size_type size() noexcept { return N; }
  static constexpr size_type max_size() noexcept { return N; }
  static constexpr bool empty() noexcept { return N == 0; }
  // element access:
  constexpr reference operator[](size_type n) { return elems_[n]; }
  constexpr const_reference operator[](size_type n) const { return elems_[n]; }
  constexpr reference at(size_type n) {
    if (n >= N) { throw std::out_of_range("array::at"); }
    return elems_[n];
  };
  constexpr const_reference at(size_type n) const {
    if (n >= N) { throw std::out_of_range("array::at"); }
    return elems_[n];
  }
  constexpr reference front() { return elems_[0]; }
  constexpr const_reference front() const { return elems_[0]; }
  constexpr reference back() { return elems_[N > 0 ? N - 1 : 0]; }
  constexpr const_reference back() const { return elems_[N > 0 ? N - 1 : 0]; }
  constexpr value_type* data() noexcept { return elems_; }
  constexpr const value_type* data() const noexcept { return elems_; }
};

template <class T, suint_t N>
constexpr bool operator==(const array<T, N>& x, const array<T, N>& y) {
  return ranges::equal(x.elems_, x.elems_ + N, y.elems_);
}
template <class T, suint_t N>
constexpr bool operator!=(const array<T, N>& x, const array<T, N>& y) {
  return !(x == y);
}
template <class T, suint_t N>
constexpr bool operator<(const array<T, N>& x, const array<T, N>& y) {
  auto&& begin0 = x.begin();
  auto&& end0   = x.end();
  auto&& begin1 = y.begin();
  auto&& end1   = y.end();

  for (; begin1 != end1; ++begin0, ++begin1) {
    if (begin0 == end0 || (*begin0) < (*begin1)) { return true; }
    if ((*begin1) < (*begin0)) { return false; }
  }
  return false;
}

template <class T, suint_t N>
constexpr bool operator>(const array<T, N>& x, const array<T, N>& y) {
  return y < x;
}
template <class T, suint_t N>
constexpr bool operator<=(const array<T, N>& x, const array<T, N>& y) {
  return !(y < x);
}
template <class T, suint_t N>
constexpr bool operator>=(const array<T, N>& x, const array<T, N>& y) {
  return !(x < y);
}

template <suint_t I, class T, suint_t N>
constexpr T& get(array<T, N>& a) noexcept {
  static_assert(I < N, "Index out of bounds in ranges::get<> (ranges::array)");
  return a.elems_[I];
}

template <suint_t I, class T, suint_t N>
constexpr const T& get(const array<T, N>& a) noexcept {
  static_assert(I < N,
                "Index out of bounds in ranges::get<> (const ranges::array)");
  return a.elems_[I];
}

template <suint_t I, class T, suint_t N>
constexpr T&& get(array<T, N>&& a) noexcept {
  static_assert(I < N,
                "Index out of bounds in ranges::get<> (ranges::array &&)");
  return std::move(a.elems_[I]);
}

static_assert(RandomAccessRange<array<int, 2>>{}, "");
}  // namespace hm3

namespace std {

#ifdef HM3_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

template <class T, hm3::suint_t N>
struct tuple_size<hm3::array<T, N>>
 : public integral_constant<hm3::suint_t, N> {};

template <size_t I, class T, hm3::suint_t N>
struct tuple_element<I, hm3::array<T, N>> {
 public:
  using type = T;
};

#ifdef HM3_COMPILER_CLANG
#pragma clang diagnostic pop
#endif

}  // namespace std
