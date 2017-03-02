#pragma once
/// \file
///
/// Integer arithmetic
///
/// TODO: benchmark non-look-up-table vs look-up-table based integer pow
#include <hm3/math/constants.hpp>
#include <hm3/math/constexpr.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/config/attributes.hpp>
#include <hm3/utility/range.hpp>

// Define this to use look-up table based integer pow
//#define HM3_IPOW_LOOKUP

namespace hm3::math {

/// Computes the logarithm log_b(n) for (b,n) integers
///
/// \warning overflows for N > b^64.
template <typename Int, CONCEPT_REQUIRES_(Integral<Int>{})>
constexpr Int ilog(Int b, Int n) {
  return n == 0 ? 0 : (n == 1 ? 0 : 1 + ilog(b, n / b));
}

/// Computes integer pow using exponentiation by squaring
/// Complexiy O(log(e))
template <typename Int, CONCEPT_REQUIRES_(Integral<Int>{})>
[[clang::no_sanitize("integer")]] constexpr Int ipow_impl(
 Int base, Int exponent) noexcept {
  Int result = 1;
  while (exponent) {
    if (exponent & 1) { result *= base; }
    exponent >>= 1;
    base *= base;
  }
  return result;
}

template <typename Int, CONCEPT_REQUIRES_(Integral<Int>{})>
[[ HM3_FLATTEN, HM3_ALWAYS_INLINE, HM3_HOT ]] constexpr Int lookup_ipow2(
 Int exponent) noexcept {
  constexpr unsigned ipow2_v[] = {1, 2, 4};
  HM3_ASSERT(e < std::extent<decltype(ipow2_v)>::value,
             "ipow2 exponent {} is out of bounds [0, {})", exponent,
             std::extent<decltype(ipow2_v)>::value);
  HM3_ASSERT(ipow2_v[exponent] == ipow_impl(Int{2}, exponent),
             "ipow2[{}] is {} but should be {}", exponent, ipow2_v[exponent],
             ipow_impl(Int{2}, exponent));
  return ipow2_v[exponent];
}

/// Computes integer pow
template <typename Int, CONCEPT_REQUIRES_(Integral<Int>{})>
[[ HM3_FLATTEN, HM3_ALWAYS_INLINE, HM3_HOT ]]  //
 constexpr Int
 ipow(Int base, Int exponent) noexcept {
#ifdef HM3_IPOW_LOOKUP
  switch (base) {
    case 0: {
      return 0;
    }
    case 1: {
      return 1;
    }
    case 2: {
      return lookup_ipow2(exponent);
    }
  }
  HM3_ASSERT(false, "unknown base {}", base);
#else
  return ipow_impl(base, exponent);
#endif
}

template <typename F, typename I,
          CONCEPT_REQUIRES_(std::is_floating_point<F>{} and Integral<I>{})>
constexpr F ipow(F base, I exponent) noexcept {
  F result = 1;
  while (exponent) {
    if (exponent & 1) { result *= base; }
    exponent >>= 1;
    base *= base;
  }
  return result;
}

/// Computes the factorial of n
///
/// \param n [in] number whose factorial will be computed
///
/// TODO: assert on overflow
template <typename Int, CONCEPT_REQUIRES_(Integral<Int>{})>
constexpr Int factorial(const Int n) noexcept {
  return (n == Int{0}) ? Int{1} : n * factorial(n - Int{1});
}

/// Computes the binomial coefficient (n m)
///
/// TODO: assert n - m >= 0 for unsigned types
template <typename Int, CONCEPT_REQUIRES_(Integral<Int>{})>
constexpr Int binomial_coefficient(const Int n, const Int m) noexcept {
  HM3_ASSERT(n - m >= 0, "n - m !>= 0 with n = {} and m = {}", n, m);
  return factorial(n) / (factorial(m) * factorial(n - m));
}

}  // namespace hm3::math
