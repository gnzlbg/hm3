#pragma once
/// \file math.hpp Run-time and Compile-time math utilities
#include <type_traits>
#include <cmath>
#include <hm3/types.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {

/// Math utilities
namespace math {

/// Takes the absolute difference between two unsigned integers (without
/// wrapping)
template <class Int, CONCEPT_REQUIRES_(UnsignedIntegral<Int>{})>
static constexpr Int absdiff(Int a, Int b) noexcept {
  static_assert(std::is_unsigned<Int>::value,
                "requires an UnsignedIntegral type");
  return (a > b) ? (a - b) : (b - a);
}

/// Computes the logarithm log_b(n) for (b,n) integers
///
/// \warning overflows for N > b^64.
template <class Int, CONCEPT_REQUIRES_(Integral<Int>{})>
constexpr Int ilog(Int b, Int n) {
  return n == 0 ? 0 : (n == 1 ? 0 : 1 + ilog(b, n / b));
}

/// Returns the signum of \p x, i.e. -1 if x < 0, +1 if x > 0, and 0
/// otherwise
template <class T,
          CONCEPT_REQUIRES_(std::is_arithmetic<T>{} and std::is_signed<T>{})>
static constexpr sint_t signum(const T& x) noexcept {
  return (T{0} < x) - (x < T{0});
}

/// Returns the signum of \p x, i.e. -1 if x < 0, +1 if x > 0, and 0
/// otherwise
template <class T,
          CONCEPT_REQUIRES_(std::is_arithmetic<T>{} and !std::is_signed<T>{})>
static constexpr sint_t signum(const T& x) noexcept {
  return T{0} < x;
}

/// Computes integer pow using exponentiation by squaring
/// Complexiy O(log(e))
template <class Int, CONCEPT_REQUIRES_(Integral<Int>{})>
constexpr Int ipow(Int b, Int e) {
  Int result = 1;
  while (e) {
    if (e & 1) { result *= b; }
    e >>= 1;
    b *= b;
  }
  return result;
}

/// Computes the factorial of n
///
/// \param n [in] number whose factorial will be computed
///
/// TODO: assert on overflow
template <class Int, CONCEPT_REQUIRES_(Integral<Int>{})>
constexpr Int factorial(const Int n) noexcept {
  return (n == Int{0}) ? Int{1} : n * factorial(n - Int{1});
}

/// Computes the binomial coefficient (n m)
///
/// TODO: assert n - m >= 0 for unsigned types
template <class Int, CONCEPT_REQUIRES_(Integral<Int>{})>
constexpr Int binomial_coefficient(const Int n, const Int m) noexcept {
  return factorial(n) / (factorial(m) * factorial(n - m));
}

/// Constexpr version of cmath floor
///
/// TODO: there has to be a better way
template <class Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr Float floor(Float x) {
  return x >= 0.0 ? int(x) : int(x) - 1;
}

}  // namespace math
}  // namespace hm3
