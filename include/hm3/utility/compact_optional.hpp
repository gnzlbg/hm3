#pragma once
/// \file
///
/// Compact Optional
///
/// Adapted from: https://github.com/akrzemi1/compact_optional
///
/// Copyright (C) 2015, Andrzej Krzemienski.
///
/// Use, modification, and distribution is subject to the Boost Software
/// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/range.hpp>
#include <utility>

namespace hm3 {

struct default_tag {};

template <typename T, typename NT = T, typename CREF = const T&>
struct compact_optional_type {
  typedef T value_type;
  typedef NT storage_type;
  typedef CREF reference_type;

  static constexpr const value_type& access_value(const storage_type& v) {
    return v;
  }
  static constexpr const value_type& store_value(const value_type& v) {
    return v;
  }
  static constexpr value_type&& store_value(value_type&& v) {
    return std::move(v);
  }
};

template <typename T, T Val>
struct empty_scalar_value : compact_optional_type<T> {
  static constexpr T empty_value() noexcept { return Val; }
  static constexpr bool is_empty_value(T v) { return v == Val; }
};

template <typename OT>
struct compact_optional_from_optional
 : compact_optional_type<typename OT::value_type, OT> {
  typedef typename OT::value_type value_type;
  typedef OT storage_type;

  static OT empty_value() noexcept { return OT(); }
  static bool is_empty_value(const OT& v) { return !v; }

  static const value_type& access_value(const storage_type& v) { return *v; }
  static storage_type store_value(const value_type& v) { return v; }
  static storage_type store_value(value_type&& v) { return std::move(v); }
};

struct compact_bool : compact_optional_type<bool, char, bool> {
  static constexpr char empty_value() noexcept { return char(2); }
  static constexpr bool is_empty_value(char v) { return v == 2; }

  static constexpr bool access_value(const char& v) { return bool(v); }
  static constexpr char store_value(const bool& v) { return v; }
};

namespace detail {

template <typename N>  //
class compact_optional_base {
 protected:
  using value_type     = typename N::value_type;
  using storage_type   = typename N::storage_type;
  using reference_type = typename N::reference_type;

  storage_type value_;

 public:
  constexpr compact_optional_base() noexcept(
   noexcept(storage_type(N::empty_value())))
   : value_(N::empty_value()) {}

  constexpr compact_optional_base(const value_type& v)
   : value_(N::store_value(v)) {
    HM3_ASSERT(has_value(), "");
  }

  constexpr compact_optional_base(value_type&& v)
   : value_(N::store_value(std::move(v))) {
    HM3_ASSERT(has_value(), "");
  }

  constexpr bool has_value() const { return !N::is_empty_value(value_); }

  constexpr reference_type value() const {
    HM3_ASSERT(has_value(), "");
    return N::access_value(value_);
  }
};

}  // namespace detail

template <typename N, typename Tag = default_tag>  //
class compact_optional : public detail::compact_optional_base<N> {
  using tag   = Tag;
  using self  = compact_optional<N, tag>;
  using super = detail::compact_optional_base<N>;
  using super::value_;

 public:
  using super::super;
  using value_type     = typename super::value_type;
  using reference_type = typename super::reference_type;

  constexpr explicit operator bool() const noexcept {
    return this->has_value();
  }

  constexpr reference_type operator*() const { return this->value(); }

  friend void swap(self& l, self& r) {
    using std::swap;
    swap(l.value_, r.value_);
  }

  CONCEPT_REQUIRES(EqualityComparable<value_type>())
  constexpr bool operator==(self const& o) const noexcept {
    return value_ == o.value_;
  }
  CONCEPT_REQUIRES(EqualityComparable<value_type>())
  constexpr bool operator!=(self const& o) const noexcept {
    return value_ != o.value_;
  }

  CONCEPT_REQUIRES(WeaklyOrdered<value_type>())
  constexpr bool operator<(self const& o) const noexcept {
    return value_ < o.value_;
  }
  CONCEPT_REQUIRES(WeaklyOrdered<value_type>())
  constexpr bool operator>(self const& o) const noexcept {
    return value_ > o.value_;
  }
  CONCEPT_REQUIRES(WeaklyOrdered<value_type>())
  constexpr bool operator<=(self const& o) const noexcept {
    return value_ <= o.value_;
  }
  CONCEPT_REQUIRES(WeaklyOrdered<value_type>())
  constexpr bool operator>=(self const& o) const noexcept {
    return value_ >= o.value_;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  self& operator++() noexcept {
    ++value_;
    return (*this);
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  self operator++(int)noexcept {
    self tmp(*this);
    ++value_;
    return tmp;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  self& operator--() noexcept {
    --value_;
    return (*this);
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  self operator--(int)noexcept {
    self tmp(*this);
    --value_;
    return tmp;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  friend self operator+(self const& a, self const& b) noexcept {
    return self{(*a) + (*b)};
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  self& operator+=(self const& other) noexcept {
    value_ += *other;
    return *this;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  self& operator-=(self const& other) noexcept {
    value_ -= *other;
    return *this;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  friend self operator-(self const& a, self const& b) noexcept {
    return self{(*a) - (*b)};
  }

  /// TODO: constrain on <<
  template <typename OStream>
  friend constexpr OStream& operator<<(OStream& os, self const& a) {
    if (a) {
      os << *a;
    } else {
      os << '_';
    }
    return os;
  };

  /// TODO: constrain on to_string
  friend std::string to_string(self const& a) {
    using std::to_string;
    return a ? to_string(*a) : std::string{"_"};
  }
};

template <typename T> using get_tag_t = typename T::tag;

}  // namespace hm3

namespace std {

template <typename N, typename Tag> struct hash<hm3::compact_optional<N, Tag>> {
  using Key         = hm3::compact_optional<N, Tag>;
  using result_type = size_t;
  using value_type  = typename N::value_type;
  size_t operator()(Key const& k) const noexcept {
    return k ? std::hash<value_type>{}(*k)
             : std::hash<value_type>{}(N::empty_value());
  }
};

// template <typename T, typename Tag, T Val>
// struct is_integral<hm3::compact_optional<hm3::empty_scalar_value<T, Val>,
// Tag>>
//  : is_integral<T> {};

}  // namespace std
