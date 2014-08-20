#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Run-time and Compile-time math utilities
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include <type_traits>
#include <hom3/core/types.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace hom3 {

/// Math utilities
namespace math {

/// Takes the absolute difference between two unsigned integers (without
/// wrapping)
///
/// \tparam T UnsignedIntegral type
template <class T>
static inline constexpr T absdiff(const T& a, const T& b) noexcept {
  static_assert(std::is_unsigned<T>::value,
                "requires an UnsignedIntegral type");
  return (a > b) ? (a - b) : (b - a);
}

namespace detail {

template <class T>
static inline constexpr SInt signum(const T& x, std::false_type) noexcept {
  return T{0} < x;
}

template <class T>
static inline constexpr SInt signum(const T& x, std::true_type) noexcept {
  return (T{0} < x) - (x < T{0});
}

}  // namespace detail

/// Returns the signum of \p x, i.e. -1 if x < 0, +1 if x > 0, and 0
/// otherwise
///
/// \tparam T ArithmeticType
template <class T> static inline constexpr SInt signum(const T& x) noexcept {
  static_assert(std::is_arithmetic<T>::value, "requires an Arithmetic type");
  return signum(x, std::is_signed<T>());
}

/// Non-standard math traits
namespace traits {

template <class T> struct is_long_int : std::false_type {};
template <> struct is_long_int<Ind> : std::true_type {};

}  // namespace traits

/// Compile-time math
namespace ct {

/// Computes the logarithm log_b(n) for (b,n) integers
///
/// Note: Overflows for N > 2^64.
constexpr long ilog(const long b, const long n) {
  return n == 0l ? 0l : n == 1l ? 0l : 1l + ilog(b, n / b);
}

/// Computes b^e for (b,e) integers
///
/// Note: overflows for e >= (64-1) / ilog(b,2);
constexpr long ipow(const long b, const long e) {
  return e == 0l ? 1l : b * ipow(b, e - 1l);
}

/// Computes the factorial of n
///
/// Note: overflows for n >= 15;
constexpr long factorial(const long n) noexcept {
  return (n == 0) ? 1 : n * factorial(n - 1);
}

constexpr long binomial_coefficient(const long n, const long m) noexcept {
  return factorial(n) / (factorial(m) * factorial(n - m));
}

}  // namespace ct

/// Run-time math
namespace rt {

namespace detail_ {

using math::traits::is_long_int;

template <class Integral1, class Integral2>
Integral1 ilog(Integral1 b, Integral2 n) {  // used only in asserts
  return n == 0 ? 0 : (n == 1 ? 0 : 1 + ilog(b, n / b));
}

template <class Integral1, class Integral2>
bool ipow_overflow_long(const Integral1 b, const Integral2 e) {
  return e >= (64 - 1) / ilog(b, static_cast<Integral1>(2));
}

template <class Integral1, class Integral2>
bool ipow_overflow_not_long(const Integral1 b, const Integral2 e) {
  return e >= (32 - 1) / ilog(b, static_cast<Integral1>(2));
}

template <class Integral1, class Integral2>
bool ipow_overflow(Integral1 b, Integral2 e) {
  return is_long_int<Integral1>::value ? ipow_overflow_long(b, e)
                                       : ipow_overflow_not_long(b, e);
}

}  // namespace detail_

/// Computes integer pow using exponentiation by squaring
/// Complexiy O(log(e))
template <class Integral1, class Integral2>
Integral1 ipow(Integral1 b, Integral2 e) {
  ASSERT(!detail_::ipow_overflow(b, e), "This expression overflows");
  Integral1 result = 1;
  while (e) {
    if (e & 1) { result *= b; }
    e >>= 1;
    b *= b;
  }
  return result;
}

}  // namespace rt

}  // namespace math

}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
