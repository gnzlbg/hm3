#pragma once
/// \file
///
/// Bounded integer type
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {

/// A tagged type with values in range [from, to)
template <typename T, T from, T to, typename Tag = void> struct bounded {
  using self           = bounded<T, from, to, Tag>;
  using value_type     = T;
  using tag            = Tag;
  using reference_type = value_type&;
  value_type value;

  constexpr bounded(T v = from) : value{std::move(v)} {
    HM3_ASSERT(value >= from and value < to,
               "value {} is out-of-bounds [{}, {})", value, from, to);
  }

  static constexpr bounded invalid() noexcept {
    bounded b;
    b.value = math::highest<T>;
    return b;
  }

  constexpr value_type operator*() const noexcept {
    HM3_ASSERT(value >= from and value < to,
               "value {} is out-of-bounds [{}, {})", value, from, to);
    return value;
  }
  constexpr reference_type operator*() noexcept {
    HM3_ASSERT(value >= from and value < to,
               "value {} is out-of-bounds [{}, {})", value, from, to);
    return value;
  }
  constexpr explicit operator bool() const noexcept {
    return value >= from and value < to;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<T>{})
  static constexpr auto rng() noexcept {
    return view::iota(from, to)
           | view::transform([](T const& i) { return bounded{i}; });
  }

  CONCEPT_REQUIRES(EqualityComparable<value_type>())
  constexpr bool operator==(self const& o) const noexcept {
    return *(*this) == *o;
  }
  CONCEPT_REQUIRES(EqualityComparable<value_type>())
  constexpr bool operator!=(self const& o) const noexcept {
    return *(*this) != *o;
  }

  CONCEPT_REQUIRES(WeaklyOrdered<value_type>())
  constexpr bool operator<(self const& o) const noexcept {
    return *(*this) < *o;
  }
  CONCEPT_REQUIRES(WeaklyOrdered<value_type>())
  constexpr bool operator>(self const& o) const noexcept {
    return *(*this) > *o;
  }
  CONCEPT_REQUIRES(WeaklyOrdered<value_type>())
  constexpr bool operator<=(self const& o) const noexcept {
    return *(*this) <= *o;
  }
  CONCEPT_REQUIRES(WeaklyOrdered<value_type>())
  constexpr bool operator>=(self const& o) const noexcept {
    return *(*this) >= *o;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  self& operator++() noexcept {
    ++value;
    HM3_ASSERT(value >= from and value < to,
               "value {} is out-of-bounds [{}, {})", value, from, to);
    return (*this);
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  self operator++(int)noexcept {
    self tmp(*this);
    ++value;
    HM3_ASSERT(value >= from and value < to,
               "value {} is out-of-bounds [{}, {})", value, from, to);
    return tmp;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  friend self operator+(self const& a, self const& b) noexcept {
    self v{(*a) + (*b)};
    return v;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  self& operator+=(self const& other) noexcept {
    value += *other;
    HM3_ASSERT(value >= from and value < to,
               "value {} is out-of-bounds [{}, {})", value, from, to);
    return *this;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  self& operator-=(self const& other) noexcept {
    value -= *other;
    HM3_ASSERT(value >= from and value < to,
               "value {} is out-of-bounds [{}, {})", value, from, to);
    return *this;
  }

  template <typename OStream>
  friend constexpr OStream& operator<<(OStream& os, self const& a) {
    os << *a;
    return os;
  }
};

}  // namespace hm3
