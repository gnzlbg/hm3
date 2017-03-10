// Copyright (C) 2011 - 2012 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// The idea and interface is based on Boost.Optional library
// authored by Fernando Luis Cacciola Carballal
//
#pragma once

#include <functional>
#include <hm3/utility/assert.hpp>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace hm3 {

namespace optional_detail {

// 20.5.4, optional for object types
template <typename T>
class optional;

// 20.5.5, optional for lvalue reference types
template <typename T>
class optional<T&>;

// workaround: std utility functions aren't constexpr yet
template <typename T>
constexpr T&& constexpr_forward(std::remove_reference_t<T>& t) noexcept {
  return static_cast<T&&>(t);
}

template <typename T>
constexpr T&& constexpr_forward(std::remove_reference_t<T>&& t) noexcept {
  static_assert(!std::is_lvalue_reference<T>{}, "!!");
  return static_cast<T&&>(t);
}

template <typename T>
constexpr std::remove_reference_t<T>&& constexpr_move(T&& t) noexcept {
  return static_cast<std::remove_reference_t<T>&&>(t);
}

template <typename T>
struct has_overloaded_addressof_ {
  template <typename X>
  static constexpr bool has_overload(...) {
    return false;
  }

  template <typename X, size_t S = sizeof(std::declval<X&>().operator&())>
  static constexpr bool has_overload(bool) {
    return true;
  }

  constexpr static bool value = has_overload<T>(true);
};

template <typename T>
struct has_overloaded_addressof
 : std::integral_constant<bool, has_overloaded_addressof_<T>::value> {};

template <typename T, CONCEPT_REQUIRES_(!has_overloaded_addressof<T>{})>
constexpr T* static_addressof(T& ref) {
  return &ref;
}

template <typename T, CONCEPT_REQUIRES_(has_overloaded_addressof<T>{})>
T* static_addressof(T& ref) {
  return std::addressof(ref);
}

template <typename U>
struct is_not_optional {
  constexpr static bool value = true;
};

template <typename T>
struct is_not_optional<optional<T>> {
  constexpr static bool value = false;
};

constexpr struct trivial_init_t {
} trivial_init{};

// 20.5.6, In-place construction
constexpr struct in_place_t {
} in_place{};

// 20.5.7, Disengaged state indicator
struct nullopt_t {
  struct init {};
  constexpr nullopt_t(init) noexcept {}
};
constexpr nullopt_t nullopt{nullopt_t::init{}};

// 20.5.8, class bad_optional_access
// class bad_optional_access : public logic_error {
//  public:
//   explicit bad_optional_access(const string& what_arg)
//    : logic_error{what_arg} {}
//   explicit bad_optional_access(const char* what_arg) : logic_error{what_arg}
//   {}
// };

template <typename T>
union storage_t {
  unsigned char dummy_;
  T value_;

  constexpr storage_t(trivial_init_t) noexcept : dummy_(){};

  template <typename... Args>
  constexpr storage_t(Args&&... args)
   : value_(constexpr_forward<Args>(args)...) {}

  ~storage_t(){};  // NOLINT
};

template <typename T>
union constexpr_storage_t {
  unsigned char dummy_;
  T value_;

  constexpr constexpr_storage_t(trivial_init_t) noexcept : dummy_(){};

  template <typename... Args>
  constexpr constexpr_storage_t(Args&&... args)
   : value_(constexpr_forward<Args>(args)...) {}

  ~constexpr_storage_t() = default;
};

constexpr struct only_set_initialized_t {
} only_set_initialized{};

template <typename T>
struct optional_base {
  bool init_;
  storage_t<T> storage_;

  constexpr optional_base() noexcept : init_(false), storage_(trivial_init){};

  constexpr explicit optional_base(only_set_initialized_t, bool init) noexcept
   : init_(init), storage_(trivial_init){};

  explicit constexpr optional_base(const T& v) : init_(true), storage_(v) {}

  explicit constexpr optional_base(T&& v)
   : init_(true), storage_(constexpr_move(v)) {}

  template <typename... Args>
  explicit optional_base(in_place_t, Args&&... args)
   : init_(true), storage_(constexpr_forward<Args>(args)...) {}

  template <
   typename U, typename... Args,
   CONCEPT_REQUIRES_(std::is_constructible<T, std::initializer_list<U>>{})>
  explicit optional_base(in_place_t, std::initializer_list<U> il,
                         Args&&... args)
   : init_(true), storage_(il, std::forward<Args>(args)...) {}

  ~optional_base() {
    if (init_) { storage_.value_.T::~T(); }
  }
};

template <typename T>
struct constexpr_optional_base {
  bool init_{false};
  constexpr_storage_t<T> storage_;

  constexpr constexpr_optional_base() noexcept
   : init_(false), storage_(trivial_init){};

  constexpr explicit constexpr_optional_base(only_set_initialized_t,
                                             bool init) noexcept
   : init_(init), storage_(trivial_init){};

  explicit constexpr constexpr_optional_base(const T& v)
   : init_(true), storage_(v) {}

  explicit constexpr constexpr_optional_base(T&& v)
   : init_(true), storage_(constexpr_move(v)) {}

  template <typename... Args>
  explicit constexpr constexpr_optional_base(in_place_t, Args&&... args)
   : init_(true), storage_(constexpr_forward<Args>(args)...) {}

  template <
   typename U, typename... Args,
   CONCEPT_REQUIRES_(std::is_constructible<T, std::initializer_list<U>>{})>
  constexpr explicit constexpr_optional_base(in_place_t,
                                             std::initializer_list<U> il,
                                             Args&&... args)
   : init_(true), storage_(il, std::forward<Args>(args)...) {}

  ~constexpr_optional_base() = default;
};

template <typename T>
using OptionalBase
 = std::conditional_t<std::is_trivially_destructible<T>{},
                      constexpr_optional_base<T>, optional_base<T>>;

template <typename T>
class optional : private OptionalBase<T> {
  static_assert(!std::is_same<std::decay_t<T>, nullopt_t>{}, "bad T");
  static_assert(!std::is_same<std::decay_t<T>, in_place_t>{}, "bad T");

  constexpr bool initialized() const noexcept { return OptionalBase<T>::init_; }
  T* dataptr() { return std::addressof(OptionalBase<T>::storage_.value_); }
  constexpr const T* dataptr() const {
    return static_addressof(OptionalBase<T>::storage_.value_);
  }

  constexpr const T& contained_val() const & {
    return OptionalBase<T>::storage_.value_;
  }

  constexpr T&& contained_val() && {
    return std::move(OptionalBase<T>::storage_.value_);
  }
  constexpr T& contained_val() & { return OptionalBase<T>::storage_.value_; }

  void clear() noexcept {
    if (initialized()) { dataptr()->T::~T(); }
    OptionalBase<T>::init_ = false;
  }

  template <typename... Args>
  void initialize(Args&&... args) noexcept(
   noexcept(T(std::forward<Args>(args)...))) {
    assert(!OptionalBase<T>::init_);
    ::new (static_cast<void*>(dataptr())) T(std::forward<Args>(args)...);
    OptionalBase<T>::init_ = true;
  }

  template <typename U, typename... Args>
  void initialize(std::initializer_list<U> il, Args&&... args) noexcept(
   noexcept(T(il, std::forward<Args>(args)...))) {
    assert(!OptionalBase<T>::init_);
    ::new (static_cast<void*>(dataptr())) T(il, std::forward<Args>(args)...);
    OptionalBase<T>::init_ = true;
  }

 public:
  using value_type = T;

  // 20.5.5.1, constructors
  constexpr optional() noexcept : OptionalBase<T>(){};
  constexpr optional(nullopt_t) noexcept : OptionalBase<T>(){};

  optional(const optional& rhs) : OptionalBase<T>(only_set_initialized, false) {
    if (rhs.initialized()) {
      ::new (static_cast<void*>(dataptr())) T(*rhs);
      OptionalBase<T>::init_ = true;
    }
  }

  optional(optional&& rhs) noexcept(std::is_nothrow_move_constructible<T>{})
   : OptionalBase<T>(only_set_initialized, false) {
    if (rhs.initialized()) {
      ::new (static_cast<void*>(dataptr())) T(std::move(*rhs));
      OptionalBase<T>::init_ = true;
    }
  }

  constexpr optional(const T& v) : OptionalBase<T>(v) {}

  constexpr optional(T&& v) : OptionalBase<T>(constexpr_move(v)) {}

  template <typename... Args>
  constexpr explicit optional(in_place_t, Args&&... args)
   : OptionalBase<T>(in_place_t{}, constexpr_forward<Args>(args)...) {}

  template <
   typename U, typename... Args,
   CONCEPT_REQUIRES_(std::is_constructible<T, std::initializer_list<U>>{})>
  constexpr explicit optional(in_place_t, std::initializer_list<U> il,
                              Args&&... args)
   : OptionalBase<T>(in_place_t{}, il, constexpr_forward<Args>(args)...) {}

  // 20.5.4.2 Destructor
  ~optional() = default;

  // 20.5.4.3, assignment
  optional& operator=(nullopt_t) noexcept {
    clear();
    return *this;
  }

  optional& operator=(const optional& rhs) {
    if (initialized() == true && rhs.initialized() == false) {
      clear();
    } else if (initialized() == false && rhs.initialized() == true) {
      initialize(*rhs);
    } else if (initialized() == true && rhs.initialized() == true) {
      contained_val() = *rhs;
    }
    return *this;
  }

  optional& operator=(optional&& rhs) noexcept(
   std::is_nothrow_move_assignable<T>{}
   && std::is_nothrow_move_constructible<T>{}) {
    if (initialized() == true && rhs.initialized() == false) {
      clear();
    } else if (initialized() == false && rhs.initialized() == true) {
      initialize(std::move(*rhs));
    } else if (initialized() == true && rhs.initialized() == true) {
      contained_val() = std::move(*rhs);
    }
    return *this;
  }

  template <typename U>
  auto operator=(U&& v)
   -> std::enable_if_t<std::is_same<std::remove_reference_t<U>, T>{},
                       optional&> {
    if (initialized()) {
      contained_val() = std::forward<U>(v);
    } else {
      initialize(std::forward<U>(v));
    }
    return *this;
  }

  template <typename... Args>
  void emplace(Args&&... args) {
    clear();
    initialize(std::forward<Args>(args)...);
  }

  template <typename U, typename... Args>
  void emplace(std::initializer_list<U> il, Args&&... args) {
    clear();
    initialize<U, Args...>(il, std::forward<Args>(args)...);
  }

  // 20.5.4.4 Swap
  void swap(optional<T>& rhs) noexcept(std::is_nothrow_move_constructible<T>{}
                                       && noexcept(swap(std::declval<T&>(),
                                                        std::declval<T&>()))) {
    if (initialized() == true && rhs.initialized() == false) {
      rhs.initialize(std::move(**this));
      clear();
    } else if (initialized() == false && rhs.initialized() == true) {
      initialize(std::move(*rhs));
      rhs.clear();
    } else if (initialized() == true && rhs.initialized() == true) {
      using std::swap;
      swap(**this, *rhs);
    }
  }

  // 20.5.4.5 Observers

  constexpr explicit operator bool() const noexcept { return initialized(); }

  constexpr T const* operator->() const {
    HM3_ASSERT(initialized(), "optional uninitialized");
    return dataptr();
  }

  constexpr T* operator->() {
    assert(initialized());
    return dataptr();
  }

  constexpr T const& operator*() const & {
    HM3_ASSERT(initialized(), "optional uninitialized");
    return contained_val();
  }

  constexpr T& operator*() & {
    HM3_ASSERT(initialized(), "optional uninitialized");
    return contained_val();
  }

  constexpr T&& operator*() && {
    HM3_ASSERT(initialized(), "optional uninitialized");
    return constexpr_move(contained_val());
  }

  constexpr T const& value() const & {
    HM3_ASSERT(initialized(), "bad optional access");
    return contained_val();
  }

  constexpr T& value() & {
    HM3_ASSERT(initialized(), "bad optional access");
    return contained_val();
  }

  constexpr T&& value() && {
    HM3_ASSERT(initialized(), "bad optional access");
    return std::move(contained_val());
  }

  template <typename V>
  constexpr T value_or(V&& v) const & {
    return *this ? **this : static_cast<T>(constexpr_forward<V>(v));
  }

  template <typename V>
  constexpr T value_or(V&& v) && {
    return *this
            ? constexpr_move(const_cast<optional<T>&>(*this).contained_val())
            : static_cast<T>(constexpr_forward<V>(v));
  }
};

template <typename T>
class optional<T&> {
  static_assert(!std::is_same<T, nullopt_t>{}, "bad T");
  static_assert(!std::is_same<T, in_place_t>{}, "bad T");
  T* ref_;

 public:
  // 20.5.5.1, construction/destruction
  constexpr optional() noexcept : ref_(nullptr) {}

  constexpr optional(nullopt_t) noexcept : ref_(nullptr) {}

  constexpr optional(T& v) noexcept : ref_(static_addressof(v)) {}

  optional(T&&) = delete;

  constexpr optional(const optional& rhs) noexcept
   : ref_(rhs.ref_) {}  // NOLINT

  explicit constexpr optional(in_place_t, T& v) noexcept
   : ref_(static_addressof(v)) {}

  explicit optional(in_place_t, T&&) = delete;

  ~optional() = default;

  // 20.5.5.2, mutation
  optional& operator=(nullopt_t) noexcept {
    ref_ = nullptr;
    return *this;
  }

  // optional& operator=(const optional& rhs) noexcept {
  // ref_ = rhs.ref_;
  // return *this;
  // }

  // optional& operator=(optional&& rhs) noexcept {
  // ref_ = rhs.ref_;
  // return *this;
  // }

  template <typename U>
  auto operator=(U&& rhs) noexcept ->  // NOLINT
   std::enable_if_t<std::is_same<std::decay_t<U>, optional<T&>>{}, optional&> {
    ref_ = rhs.ref_;
    return *this;
  }

  template <typename U>
  auto operator=(U&& rhs) noexcept
   -> std::enable_if_t<!std::is_same<std::decay_t<U>, optional<T&>>{},
                       optional&> = delete;

  void emplace(T& v) noexcept { ref_ = static_addressof(v); }

  void emplace(T&&) = delete;

  void swap(optional<T&>& rhs) noexcept { std::swap(ref_, rhs.ref_); }

  // 20.5.5.3, observers
  constexpr T* operator->() const {
    HM3_ASSERT(ref_ != nullptr, "optional uninitialized");
    return ref_;
  }

  constexpr T& operator*() const {
    HM3_ASSERT(ref_ != nullptr, "optional uninitialized");
    return *ref_;
  }

  constexpr T& value() const {
    HM3_ASSERT(ref_ != nullptr, "bad optional access");
    return *ref_;
  }

  explicit constexpr operator bool() const noexcept { return ref_ != nullptr; }

  template <typename V>
  constexpr std::decay_t<T> value_or(V&& v) const {
    return *this ? **this
                 : static_cast<std::decay_t<T>>(constexpr_forward<V>(v));
  }
};

template <typename T>
class optional<T&&> {
  static_assert(sizeof(T) == 0, "optional rvalue referencs disallowed");
};

// 20.5.8, Relational operators
template <typename T>
constexpr bool operator==(const optional<T>& x, const optional<T>& y) {
  return bool(x) != bool(y) ? false : bool(x) == false ? true : *x == *y;
}

template <typename T>
constexpr bool operator!=(const optional<T>& x, const optional<T>& y) {
  return !(x == y);
}

template <typename T>
constexpr bool operator<(const optional<T>& x, const optional<T>& y) {
  return (!y) ? false : (!x) ? true : *x < *y;
}

template <typename T>
constexpr bool operator>(const optional<T>& x, const optional<T>& y) {
  return (y < x);
}

template <typename T>
constexpr bool operator<=(const optional<T>& x, const optional<T>& y) {
  return !(y < x);
}

template <typename T>
constexpr bool operator>=(const optional<T>& x, const optional<T>& y) {
  return !(x < y);
}

// 20.5.9 Comparison with nullopt
template <typename T>
constexpr bool operator==(const optional<T>& x, nullopt_t) noexcept {
  return (!x);
}

template <typename T>
constexpr bool operator==(nullopt_t, const optional<T>& x) noexcept {
  return (!x);
}

template <typename T>
constexpr bool operator!=(const optional<T>& x, nullopt_t) noexcept {
  return bool(x);
}

template <typename T>
constexpr bool operator!=(nullopt_t, const optional<T>& x) noexcept {
  return bool(x);
}

template <typename T>
constexpr bool operator<(const optional<T>&, nullopt_t) noexcept {
  return false;
}

template <typename T>
constexpr bool operator<(nullopt_t, const optional<T>& x) noexcept {
  return bool(x);
}

template <typename T>
constexpr bool operator<=(const optional<T>& x, nullopt_t) noexcept {
  return (!x);
}

template <typename T>
constexpr bool operator<=(nullopt_t, const optional<T>&) noexcept {
  return true;
}

template <typename T>
constexpr bool operator>(const optional<T>& x, nullopt_t) noexcept {
  return bool(x);
}

template <typename T>
constexpr bool operator>(nullopt_t, const optional<T>&) noexcept {
  return false;
}

template <typename T>
constexpr bool operator>=(const optional<T>&, nullopt_t) noexcept {
  return true;
}

template <typename T>
constexpr bool operator>=(nullopt_t, const optional<T>& x) noexcept {
  return (!x);
}

// 20.5.10, Comparison with T
template <typename T>
constexpr bool operator==(const optional<T>& x, const T& v) {
  return bool(x) ? *x == v : false;
}

template <typename T>
constexpr bool operator==(const T& v, const optional<T>& x) {
  return bool(x) ? v == *x : false;
}

template <typename T>
constexpr bool operator!=(const optional<T>& x, const T& v) {
  return bool(x) ? *x != v : true;
}

template <typename T>
constexpr bool operator!=(const T& v, const optional<T>& x) {
  return bool(x) ? v != *x : true;
}

template <typename T>
constexpr bool operator<(const optional<T>& x, const T& v) {
  return bool(x) ? *x < v : true;
}

template <typename T>
constexpr bool operator>(const T& v, const optional<T>& x) {
  return bool(x) ? v > *x : true;
}

template <typename T>
constexpr bool operator>(const optional<T>& x, const T& v) {
  return bool(x) ? *x > v : false;
}

template <typename T>
constexpr bool operator<(const T& v, const optional<T>& x) {
  return bool(x) ? v < *x : false;
}

template <typename T>
constexpr bool operator>=(const optional<T>& x, const T& v) {
  return bool(x) ? *x >= v : false;
}

template <typename T>
constexpr bool operator<=(const T& v, const optional<T>& x) {
  return bool(x) ? v <= *x : false;
}

template <typename T>
constexpr bool operator<=(const optional<T>& x, const T& v) {
  return bool(x) ? *x <= v : true;
}

template <typename T>
constexpr bool operator>=(const T& v, const optional<T>& x) {
  return bool(x) ? v >= *x : true;
}

// Comparison of optionsl<T&> with T
template <typename T>
constexpr bool operator==(const optional<T&>& x, const T& v) {
  return bool(x) ? *x == v : false;
}

template <typename T>
constexpr bool operator==(const T& v, const optional<T&>& x) {
  return bool(x) ? v == *x : false;
}

template <typename T>
constexpr bool operator!=(const optional<T&>& x, const T& v) {
  return bool(x) ? *x != v : true;
}

template <typename T>
constexpr bool operator!=(const T& v, const optional<T&>& x) {
  return bool(x) ? v != *x : true;
}

template <typename T>
constexpr bool operator<(const optional<T&>& x, const T& v) {
  return bool(x) ? *x < v : true;
}

template <typename T>
constexpr bool operator>(const T& v, const optional<T&>& x) {
  return bool(x) ? v > *x : true;
}

template <typename T>
constexpr bool operator>(const optional<T&>& x, const T& v) {
  return bool(x) ? *x > v : false;
}

template <typename T>
constexpr bool operator<(const T& v, const optional<T&>& x) {
  return bool(x) ? v < *x : false;
}

template <typename T>
constexpr bool operator>=(const optional<T&>& x, const T& v) {
  return bool(x) ? *x >= v : false;
}

template <typename T>
constexpr bool operator<=(const T& v, const optional<T&>& x) {
  return bool(x) ? v <= *x : false;
}

template <typename T>
constexpr bool operator<=(const optional<T&>& x, const T& v) {
  return bool(x) ? *x <= v : true;
}

template <typename T>
constexpr bool operator>=(const T& v, const optional<T&>& x) {
  return bool(x) ? v >= *x : true;
}

// Comparison of optionsl<T const&> with T
template <typename T>
constexpr bool operator==(const optional<const T&>& x, const T& v) {
  return bool(x) ? *x == v : false;
}

template <typename T>
constexpr bool operator==(const T& v, const optional<const T&>& x) {
  return bool(x) ? v == *x : false;
}

template <typename T>
constexpr bool operator!=(const optional<const T&>& x, const T& v) {
  return bool(x) ? *x != v : true;
}

template <typename T>
constexpr bool operator!=(const T& v, const optional<const T&>& x) {
  return bool(x) ? v != *x : true;
}

template <typename T>
constexpr bool operator<(const optional<const T&>& x, const T& v) {
  return bool(x) ? *x < v : true;
}

template <typename T>
constexpr bool operator>(const T& v, const optional<const T&>& x) {
  return bool(x) ? v > *x : true;
}

template <typename T>
constexpr bool operator>(const optional<const T&>& x, const T& v) {
  return bool(x) ? *x > v : false;
}

template <typename T>
constexpr bool operator<(const T& v, const optional<const T&>& x) {
  return bool(x) ? v < *x : false;
}

template <typename T>
constexpr bool operator>=(const optional<const T&>& x, const T& v) {
  return bool(x) ? *x >= v : false;
}

template <typename T>
constexpr bool operator<=(const T& v, const optional<const T&>& x) {
  return bool(x) ? v <= *x : false;
}

template <typename T>
constexpr bool operator<=(const optional<const T&>& x, const T& v) {
  return bool(x) ? *x <= v : true;
}

template <typename T>
constexpr bool operator>=(const T& v, const optional<const T&>& x) {
  return bool(x) ? v >= *x : true;
}

// 20.5.12 Specialized algorithms
template <typename T>
void swap(optional<T>& x, optional<T>& y) noexcept(noexcept(x.swap(y))) {
  x.swap(y);
}

template <typename T>
constexpr optional<std::decay_t<T>> make_optional(T&& v) {
  return optional<std::decay_t<T>>(constexpr_forward<T>(v));
}

template <typename X>
constexpr optional<X&> make_optional(std::reference_wrapper<X> v) {
  return optional<X&>(v.get());
}

}  // namespace optional_detail

template <typename T>
using optional = optional_detail::optional<T>;

using optional_detail::nullopt;
using optional_detail::in_place;
using optional_detail::make_optional;

}  // namespace hm3

namespace std {
template <typename T>
struct hash<hm3::optional_detail::optional<T>> {
  using result_type   = typename hash<T>::result_type;
  using argument_type = hm3::optional_detail::optional<T>;

  constexpr result_type operator()(argument_type const& arg) const {
    return arg ? std::hash<T>{}(*arg) : result_type{};
  }
};

template <typename T>
struct hash<hm3::optional_detail::optional<T&>> {
  using result_type   = typename hash<T>::result_type;
  using argument_type = hm3::optional_detail::optional<T&>;

  constexpr result_type operator()(argument_type const& arg) const {
    return arg ? std::hash<T>{}(*arg) : result_type{};
  }
};
}  // namespace std
