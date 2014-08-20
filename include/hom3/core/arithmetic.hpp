#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Arithmetic type implementation (without implicit conversions)
////////////////////////////////////////////////////////////////////////////////
#include <type_traits>
#include <limits>
#include <algorithm>
#include <hom3/core/constants.hpp>
#include <boost/detail/numeric_traits.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace hom3 {

/// Arithmetic type without implicit conversions
template <class T, class B = void> struct arithmetic {
  using value_type = T;
  using type = T;

  /// \name Asignment operators
  ///@{
  constexpr arithmetic() noexcept : value{invalid} {};
  constexpr arithmetic(const arithmetic& other) = default;
  constexpr arithmetic(arithmetic&& other) = default;
  constexpr arithmetic& operator=(const arithmetic& other) = default;
  constexpr arithmetic& operator=(arithmetic&& other) = default;

  constexpr explicit arithmetic(const T& other) noexcept(
   std::is_nothrow_constructible<T>::value)
   : value{other} {}
  template <class U, class V>
  constexpr explicit arithmetic(const arithmetic<U, V>& other) noexcept(
   std::is_nothrow_constructible<T>::value)
   : value(other.value) {}
  constexpr arithmetic& operator=(const T& other) noexcept {
    value = other;
    return *this;
  }
  constexpr arithmetic& operator=(constant::invalid) noexcept {
    value = invalid;
    return *this;
  }
  ///@}

  /// \name Conversion operators
  ///@{
  explicit constexpr operator T() noexcept { return value; }
  explicit constexpr operator const T() const noexcept { return value; }

  template <class U, class V>
  explicit constexpr operator arithmetic<U, V>() noexcept {
    return value;
  }

  template <class U, class V>
  explicit constexpr operator const arithmetic<U, V>() const noexcept {
    return value;
  }
  ///@}

  /// \name Compound assignment +=, -=, *=, /=
  ///@{
  constexpr arithmetic& operator+=(const arithmetic& other) noexcept {
    value += other.value;
    return *this;
  }
  constexpr arithmetic& operator-=(const arithmetic& other) noexcept {
    value -= other.value;
    return *this;
  }
  constexpr arithmetic& operator+=(const T& other) noexcept {
    value += other;
    return *this;
  }
  constexpr arithmetic& operator-=(const T& other) noexcept {
    value -= other;
    return *this;
  }
  constexpr arithmetic& operator*=(const arithmetic& other) noexcept {
    value *= other.value;
    return *this;
  }
  constexpr arithmetic& operator/=(const arithmetic& other) noexcept {
    value /= other.value;
    return *this;
  }
  ///@}

  /// \name Prefix increment operators ++(),--()
  ///@{
  constexpr arithmetic& operator++() noexcept {
    ++value;
    return *this;
  }
  constexpr arithmetic& operator--() noexcept {
    --value;
    return *this;
  }
  ///@}

  /// \name Postfix increment operators ()++,()--
  ///@{
  constexpr arithmetic operator++(int) noexcept {
    arithmetic tmp(*this);
    ++(*this);
    return tmp;
  }
  constexpr arithmetic operator--(int) noexcept {
    arithmetic tmp(*this);
    --(*this);
    return tmp;
  }
  ///@}

  /// \name Access operator
  ///@{
  constexpr T& operator()() & noexcept { return value; }
  constexpr T operator()() && noexcept { return value; }
  constexpr T operator()() const& noexcept { return value; }
  ///@}

  /// Data (wrapped value):
  T value;

  /// \name Comparison operators ==, !=, <, >, <=, >=
  /// \relates arithmetic<T, U>
  ///@{
  constexpr bool operator==(const constant::invalid) {
    return value == T{invalid};
  }
};
///@}

/// swap
/// \relates arithmetic<T, U>
template <class T, class U>
constexpr void swap(arithmetic<T, U>&& a, arithmetic<T, U>&& b) noexcept {
  using std::swap;
  swap(a.value, b.value);
}

/// \name arithmetic operators +,-,*,/,unary -
/// \relates arithmetic<T, U>
///@{
template <class T, class U>
constexpr arithmetic<T, U> operator+(arithmetic<T, U> a,
                                     const arithmetic<T, U>& b) noexcept {
  return a += b;
}
template <class T, class U>
constexpr arithmetic<T, U> operator-(arithmetic<T, U> a,
                                     const arithmetic<T, U>& b) noexcept {
  return a -= b;
}
template <class T, class U>
constexpr arithmetic<T, U> operator*(arithmetic<T, U> a,
                                     const arithmetic<T, U>& b) noexcept {
  return a *= b;
}
template <class T, class U>
constexpr arithmetic<T, U> operator/(arithmetic<T, U> a,
                                     const arithmetic<T, U>& b) noexcept {
  return a /= b;
}

template <class T, class U>
constexpr arithmetic<T, U> operator-(arithmetic<T, U> const& other) noexcept {
  return arithmetic<T, U>{-other.value};
}
///@}

/// \name Comparison operators ==, !=, <, >, <=, >=
/// \relates arithmetic<T, U>
///@{
template <class T, class U>
constexpr bool operator==(const arithmetic<T, U>& a,
                          const arithmetic<T, U>& b) noexcept {
  return a.value == b.value;
}
template <class T, class U>
constexpr bool operator<(const arithmetic<T, U>& a,
                         const arithmetic<T, U>& b) noexcept {
  return a.value < b.value;
}
template <class T, class U>
constexpr bool operator<=(const arithmetic<T, U>& a,
                          const arithmetic<T, U>& b) noexcept {
  return a < b || a == b;
}
template <class T, class U>
constexpr bool operator!=(const arithmetic<T, U>& a,
                          const arithmetic<T, U>& b) noexcept {
  return !(a == b);
}
template <class T, class U>
constexpr bool operator>(const arithmetic<T, U>& a,
                         const arithmetic<T, U>& b) noexcept {
  return !(a <= b);
}
template <class T, class U>
constexpr bool operator>=(const arithmetic<T, U>& a,
                          const arithmetic<T, U>& b) noexcept {
  return !(a < b);
}

/// \name Pointer arithmetic
///@{

template <class T, class U, class V,
          enable_if<std::is_pointer<T>> = traits::dummy>
constexpr auto operator+(T a, const arithmetic<U, V>& i) RETURNS(a + i());

template <class T, class U, class V,
          traits::enable_if<std::is_array<T>> = traits::dummy>
constexpr auto operator+(T a, const arithmetic<U, V>& i) RETURNS(a + i());

template <class T, class U, class V,
          traits::enable_if<std::is_pointer<T>> = traits::dummy>
constexpr auto operator-(T a, const arithmetic<U, V>& i) RETURNS(a - i());

template <class T, class U, class V,
          traits::enable_if<std::is_array<T>> = traits::dummy>
constexpr auto operator-(T a, const arithmetic<U, V>& i) RETURNS(a - i());

///@}

/// istream operator
template <class C, class CT, class T, class B>
inline auto operator>>(std::basic_istream<C, CT>& i, arithmetic<T, B>& v)
 -> decltype(i >> v()) {
  return i >> v();
}

/// ostream operator
template <class C, class CT, class T, class B>
inline auto operator<<(std::basic_ostream<C, CT>& o, const arithmetic<T, B>& v)
 -> decltype(o << v()) {
  return o << v();
}

/// to_string
template <class T, class U>
inline std::string to_string(const arithmetic<T, U> a) {
  return std::to_string(a());
}

template <class T> struct is_integer_ { static const bool value = false; };
template <class T, class U> struct is_integer_<arithmetic<T, U>> {
  static const bool value = true;
};

template <class T> struct is_integer {
  static const bool value
   = is_integer_<std::remove_reference_t<std::remove_cv_t<T>>>::value;
};

template <class T, enable_if<is_integer<T>> = traits::dummy>
constexpr inline auto&& primitive_cast(T&& t) {
  static_assert(is_integer<T>::value, "T must be an integer!");
  return t();
}

template <class T, enable_if<is_integer<T>> = traits::dummy>
constexpr inline auto primitive_cast(const T& t) -> decltype(t()) {
  static_assert(is_integer<T>::value, "T must be an integer!");
  return t();
}

template <class T, enable_if<is_integer<T>> = traits::dummy>
constexpr inline auto& primitive_cast(T& t) {
  static_assert(is_integer<T>::value, "T must be an integer!");
  return t();
}

template <class T, disable_if<is_integer<T>> = traits::dummy>
constexpr inline auto&& primitive_cast(T&& t) {
  static_assert(!is_integer<T>::value, "T can't be an integer!");
  return std::forward<T>(t);
}

template <class T, disable_if<is_integer<T>> = traits::dummy>
constexpr inline auto primitive_cast(const T& t) -> decltype(t) {
  static_assert(!is_integer<T>::value, "T can't be an integer!");
  return t;
}

template <class T, disable_if<is_integer<T>> = traits::dummy>
constexpr inline auto& primitive_cast(T& t) {
  static_assert(!is_integer<T>::value, "T can't be an integer!");
  return t;
}

////////////////////////////////////////////////////////////////////////////////
}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////

/// \name arithmetic types are numeric types
/// \relates arithmetic<T, U>
///@{
namespace std {

template <class T, class B>
class numeric_limits<hom3::arithmetic<T, B>> : public numeric_limits<T> {
 public:
  static constexpr bool is_specialized = true;
};

}  // namespace std
///@}

namespace boost {

namespace detail {

template <class T, class U> struct numeric_traits<hom3::arithmetic<T, U>> {
  using difference_type = T;
};

}  // namespace detail

}  // namespace boost
////////////////////////////////////////////////////////////////////////////////
