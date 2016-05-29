#pragma once
/// \file
///
/// Math utilities
#include <cmath>
#include <hm3/types.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/matrix.hpp>
#include <hm3/utility/range.hpp>
#include <limits>
#include <type_traits>

//#define HM3_IPOW_LOOKUP

namespace hm3 {

/// Math utilities
namespace math {

/// Numeric constants
namespace constant {

/// Epsilon
template <typename T>
static const constexpr T eps = std::numeric_limits<T>::epsilon();

/// Pi
template <typename T>
static const constexpr T pi
 = 3.141592653589793238462643383279502884197169399375105820974944;

/// Euler's number
template <typename T>
static const constexpr T e
 = 2.7182818284590452353602874713526624977572470936999595749669676;

}  // namespace constant

using constant::pi;
using constant::e;
using constant::eps;

/// Takes the absolute difference between two unsigned integers (without
/// wrapping)
template <typename Int, CONCEPT_REQUIRES_(UnsignedIntegral<Int>{})>
static constexpr Int absdiff(Int a, Int b) noexcept {
  static_assert(std::is_unsigned<Int>::value,
                "requires an UnsignedIntegral type");
  return (a > b) ? (a - b) : (b - a);
}

/// Computes the logarithm log_b(n) for (b,n) integers
///
/// \warning overflows for N > b^64.
template <typename Int, CONCEPT_REQUIRES_(Integral<Int>{})>
constexpr Int ilog(Int b, Int n) {
  return n == 0 ? 0 : (n == 1 ? 0 : 1 + ilog(b, n / b));
}

/// Returns the signum of \p x, i.e. -1 if x < 0, +1 if x > 0, and 0
/// otherwise
template <typename T,
          CONCEPT_REQUIRES_(std::is_arithmetic<T>{} and std::is_signed<T>{})>
static constexpr sint_t signum(const T& x) noexcept {
  return (T{0} < x) - (x < T{0});
}

/// Returns the signum of \p x, i.e. -1 if x < 0, +1 if x > 0, and 0
/// otherwise
template <typename T,
          CONCEPT_REQUIRES_(std::is_arithmetic<T>{} and !std::is_signed<T>{})>
static constexpr sint_t signum(const T& x) noexcept {
  return T{0} < x;
}

/// Computes integer pow using exponentiation by squaring
/// Complexiy O(log(e))
template <typename Int, CONCEPT_REQUIRES_(Integral<Int>{})>
[[clang::no_sanitize("integer")]] constexpr Int ipow_impl(Int b, Int e) {
  Int result = 1;
  while (e) {
    if (e & 1) { result *= b; }
    e >>= 1;
    b *= b;
  }
  return result;
}

template <typename Int, CONCEPT_REQUIRES_(Integral<Int>{})>
[[ HM3_FLATTEN, HM3_ALWAYS_INLINE, HM3_HOT ]] constexpr Int lookup_ipow2(
 Int e) {
  constexpr unsigned ipow2_v[] = {1, 2, 4};
  HM3_ASSERT(e < std::extent<decltype(ipow2_v)>::value,
             "ipow2 exponent {} is out of bounds [0, {})", e,
             std::extent<decltype(ipow2_v)>::value);
  HM3_ASSERT(ipow2_v[e] == ipow_impl(Int{2}, e),
             "ipow2[{}] is {} but should be {}", e, ipow2_v[e],
             ipow_impl(Int{2}, e));
  return ipow2_v[e];
}

/// Computes integer pow
template <typename Int, CONCEPT_REQUIRES_(Integral<Int>{})>
[[ HM3_FLATTEN, HM3_ALWAYS_INLINE, HM3_HOT ]]  //
 constexpr Int
 ipow(Int b, Int e) {
#ifdef HM3_IPOW_LOOKUP
  switch (b) {
    case 0: {
      return 0;
    }
    case 1: {
      return 1;
    }
    case 2: {
      return lookup_ipow2(e);
    }
  }
  HM3_ASSERT(false, "unknown base {}", b);
#else
  return ipow_impl(b, e);
#endif
}

/// Sign of a floating point number (doesn't handle NaNs)
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr Float fast_sign(Float f) noexcept {
  return (f < Float{0}) ? Float{-1} : Float{1};
}

/// Sign of a floating point number (handles NaNs)
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr Float robust_sign(Float f) noexcept {
  return std::signbit(f) ? Float{-1} : Float{1};
}

/// Sign of a floating point number
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr Float sign(Float f) noexcept {
  HM3_ASSERT(!std::isnan(f), "");
  return fast_sign(f);
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

/// Integer floor
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr int64_t ifloor(Float x) {
  HM3_ASSERT(!std::isnan(x), "floor of nan");
  HM3_ASSERT(!std::isinf(x), "floor of infinity");
  int64_t i = x;
  return i - (x < i);
}

/// Integer ceil
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr int64_t iceil(Float x) {
  HM3_ASSERT(!std::isnan(x), "ceil of nan");
  HM3_ASSERT(!std::isinf(x), "ceil of infinity");
  int64_t i = x;
  return i + (x > i);
}

/// Constexpr version of cmath std::floor
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr Float floor(Float x) {
  return ifloor(x);
}

/// Constexpr version of cmath std::ceil
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr Float ceil(Float x) {
  return iceil(x);
}

namespace math_detail {

// The following code is from the Google Test Testing Framework.
// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Authors: wan@google.com (Zhanyong Wan), eefacm@gmail.com (Sean Mcafee)
//
// The Google C++ Testing Framework (Google Test)

/// This template class serves as a compile-time function from \tparam
/// Size to type.  It maps a size in bytes to a primitive type with that
/// size. e.g.
///
/// \code
///   type_with_size<4>::UInt
/// \endcode
///
/// is typedef-ed to be unsigned int (unsigned integer made up of 4
/// bytes).
///
/// Such functionality should belong to STL, but I cannot find it
/// there.
///
/// Google Test uses this class in the implementation of floating-point
/// comparison.
///
/// For now it only handles UInt (unsigned int) as that's all Google Test
/// needs.  Other types can be easily added in the future if need
/// arises.
template <std::size_t Size> struct type_with_size {
  /// This prevents the user from using type_with_size<N> with incorrect
  /// values of N.
  using UInt = void;
};

/// The specialization for size 4.
template <> struct type_with_size<4> {
  /// unsigned int has size 4 in both gcc and MSVC.
  ///
  /// As base/basictypes.h doesn't compile on Windows, we cannot use
  /// uint32, uint64, and etc here.
  using Int  = int;
  using UInt = unsigned int;
};

/// The specialization for size 8.
template <> struct type_with_size<8> {
  using Int  = long long;
  using UInt = unsigned long long;
};

/// This template class represents an IEEE floating-point number
/// (either single-precision or double-precision, depending on the
/// template parameters).
///
/// The purpose of this class is to do more sophisticated number
/// comparison.  (Due to round-off error, etc, it's very unlikely that
/// two floating-points will be equal exactly.  Hence a naive
/// comparison by the == operation often doesn't work.)
///
/// Format of IEEE floating-point:
///
///   The most-significant bit being the leftmost, an IEEE
///   floating-point looks like
///
///     sign_bit exponent_bits fraction_bits
///
///   Here, sign_bit is a single bit that designates the sign of the
///   number.
///
///   For float, there are 8 exponent bits and 23 fraction bits.
///
///   For double, there are 11 exponent bits and 52 fraction bits.
///
///   More details can be found at
///   http://en.wikipedia.org/wiki/IEEE_floating-point_standard.
///
/// Template parameter:
///
/// \tparam RawType The raw floating-point type (either float or double)
template <typename RawType> struct floating_point {
  /// Defines the unsigned integer type that has the same size as the
  /// floating point number.
  using Bits = typename type_with_size<sizeof(RawType)>::UInt;

  /// Constants.

  /// # of bits in a number.
  static const constexpr std::size_t k_bit_count = 8 * sizeof(RawType);

  /// # of fraction bits in a number.
  static const constexpr std::size_t k_fraction_bit_count
   = std::numeric_limits<RawType>::digits - 1;

  /// # of exponent bits in a number.
  static const constexpr std::size_t k_exponent_bit_count
   = k_bit_count - 1 - k_fraction_bit_count;

  /// The mask for the sign bit.
  static const constexpr Bits k_sign_bit_mask = static_cast<Bits>(1)
                                                << (k_bit_count - 1);

  // The mask for the fraction bits.
  static const constexpr Bits k_fraction_bit_mask
   = (~static_cast<Bits>(0) >> (k_exponent_bit_count + 1));

  /// The mask for the exponent bits.
  static const constexpr Bits k_exponent_bit_mask
   = ~(k_sign_bit_mask | k_fraction_bit_mask);

  /// How many ULP's (Units in the Last Place) we want to tolerate when
  /// comparing two numbers.  The larger the value, the more error we
  /// allow.  A 0 value means that two numbers must be exactly the same
  /// to be considered equal.
  ///
  /// The maximum error of a single floating-point operation is 0.5
  /// units in the last place.  On Intel CPU's, all floating-point
  /// calculations are done with 80-bit precision, while double has 64
  /// bits.  Therefore, 4 should be enough for ordinary use.
  ///
  /// See the following article for more details on ULP:
  /// http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm.
  static const constexpr std::size_t k_max_ulps = 4;

  /// Constructs a floating_point from a raw floating-point number.
  ///
  /// On an Intel CPU, passing a non-normalized NAN (Not a Number)
  /// around may change its bits, although the new value is guaranteed
  /// to be also a NAN.  Therefore, don't expect this constructor to
  /// preserve the bits in x when x is a NAN.
  constexpr explicit floating_point(const RawType& x) noexcept : u_{x} {}

 private:
  constexpr explicit floating_point(const Bits& b) noexcept : u_{b} {}

 public:
  /// Static methods

  /// Reinterprets a bit pattern as a floating-point number.
  ///
  /// This function is needed to test the almost_equals() method.
  static constexpr RawType reinterpret_bits(const Bits bits) noexcept {
    floating_point fp(bits);
    return fp.u_.value_;
  }

  /// Returns the floating-point number that represent positive infinity.
  static constexpr RawType infinity() noexcept {
    return reinterpret_bits(k_exponent_bit_mask);
  }

  /// Non-static methods

  /// Returns the bits that represents this number.
  constexpr const Bits& bits() const noexcept { return u_.bits_; }

  /// Returns the exponent bits of this number.
  constexpr Bits exponent_bits() const noexcept {
    return k_exponent_bit_mask & u_.bits_;
  }

  /// Returns the fraction bits of this number.
  constexpr Bits fraction_bits() const noexcept {
    return k_fraction_bit_mask & u_.bits_;
  }

  /// Returns the sign bit of this number.
  constexpr Bits sign_bit() const noexcept {
    return k_sign_bit_mask & u_.bits_;
  }

  /// Returns true iff this is NAN (not a number).
  constexpr bool is_nan() const noexcept {
    /// It's a NAN if the exponent bits are all ones and the fraction
    /// bits are not entirely zeros.
    return (exponent_bits() == k_exponent_bit_mask) && (fraction_bits() != 0);
  }

  /// Returns true iff this number is at most k_max_ulps ULP's away from
  /// rhs.  In particular, this function:
  ///
  ///   - returns false if either number is (or both are) NAN.
  ///   - treats really large numbers as almost equal to infinity.
  ///   - thinks +0.0 and -0.0 are 0 DLP's apart.
  constexpr bool almost_equals(const floating_point& rhs) const noexcept {
    /// The IEEE standard says that any comparison operation involving
    /// a NAN must return false.
    if (is_nan() || rhs.is_nan()) { return false; }

    return distance_between_sign_and_magnitude_numbers(u_.bits_, rhs.u_.bits_)
           <= k_max_ulps;
  }

 private:
  /// The data type used to store the actual floating-point number.
  union floating_point_union {
    constexpr floating_point_union(RawType val) : value_{val} {}
    constexpr floating_point_union(Bits bits) : bits_{bits} {}
    RawType value_;  ///< The raw floating-point number.
    Bits bits_;      ///< The bits that represent the number.
  };

  /// Converts an integer from the sign-and-magnitude representation to
  /// the biased representation.  More precisely, let N be 2 to the
  /// power of (k_bit_count - 1), an integer x is represented by the
  /// unsigned number x + N.
  ///
  /// For instance,
  ///
  ///   -N + 1 (the most negative number representable using
  ///          sign-and-magnitude) is represented by 1;
  ///   0      is represented by N; and
  ///   N - 1  (the biggest number representable using
  ///          sign-and-magnitude) is represented by 2N - 1.
  ///
  /// Read http://en.wikipedia.org/wiki/Signed_number_representations
  /// for more details on signed number representations.
  constexpr static Bits sign_and_magnitude_to_biased(const Bits& sam) noexcept {
    if (k_sign_bit_mask & sam) {
      /// sam represents a negative number.
      return ~sam + 1;
    }
    /// sam represents a positive number.
    return k_sign_bit_mask | sam;
  }

  /// Given two numbers in the sign-and-magnitude representation,
  /// returns the distance between them as an unsigned number.
  constexpr static Bits distance_between_sign_and_magnitude_numbers(
   const Bits& sam1, const Bits& sam2) noexcept {
    const Bits biased1 = sign_and_magnitude_to_biased(sam1);
    const Bits biased2 = sign_and_magnitude_to_biased(sam2);
    return (biased1 >= biased2) ? (biased1 - biased2) : (biased2 - biased1);
  }

  floating_point_union u_;
};

}  // namespace math_detail

/// Compares two floating point values for equality
template <typename T, typename U,
          CONCEPT_REQUIRES_(Same<T, U>{} and std::is_floating_point<T>{})>
static constexpr bool approx(T const& a, U const& b) noexcept {
  return math_detail::floating_point<T>{a}.almost_equals(
   math_detail::floating_point<T>{b});
}

template <typename T, typename U, CONCEPT_REQUIRES_(!Same<T, U>{})>
static constexpr bool approx(T const& a, U const& b) noexcept {
  return approx(num_t(a), num_t(b));
}

/// Adds a signed integral to an unsigned integral
///
/// If the integer is positive, it casts it into unsigned and adds it If the
/// integer is negative, it makes it positive, then cast it into unsigned and
/// finally substract it.
///
/// This avoids signed integer overflow (which happens when casting to unsigned
/// if the integer is negative) and thus Undefined Behavior.
///
template <typename SignedInt, typename UnsignedInt,
          CONCEPT_REQUIRES_(SignedIntegral<SignedInt>{}
                            and UnsignedIntegral<UnsignedInt>{})>
constexpr SignedInt add_signed_to_unsigned(SignedInt s,
                                           UnsignedInt u) noexcept {
  if (s > 0) {
    u += static_cast<UnsignedInt>(s);
  } else {
    u -= static_cast<UnsignedInt>(-s);
  }
  return u;
}

/// Computes an n-dimensional elliptical gaussian function
struct gaussian_fn {
  /// Computes an n-dimensional elliptical gaussian function
  ///
  /// Sigma is the spread
  /// mu is the center
  ///
  /// f(x) = \frac {1}{\sqrt{2 \pi}^Nd \prod \sigma_i} \exp{- \left ( \sum
  /// \frac{(x_i - \mu_i)^2}{2 \sigma_i^2} \right )}
  template <suint_t Nd>
  constexpr num_t operator()(num_a<Nd> x, num_a<Nd> mu, num_a<Nd> sigma) const
   noexcept {
    // compute the exponent:
    num_t exponent = 0.;
    for (suint_t d = 0; d < Nd; ++d) {
      exponent += std::pow(x(d) - mu(d), 2.) / (2. * std::pow(sigma(d), 2));
    }
    exponent *= -1.;

    // compute the front factor:
    num_t factor = 1. / std::pow(std::sqrt(2. * pi<num_t>), Nd);
    for (suint_t d = 0; d < Nd; ++d) { factor *= 1. / sigma(d); }

    return factor * std::exp(exponent);
  }

  /// Computes an n-dimensional gaussian function with constant sigma:
  ///
  /// f(x) = \frac {1}{(\sqrt{2 \pi} \sigma)^Nd} \exp{- \left ( \sum
  /// (x_i - \mu_i)^2 \right ) / (2 \sigma^2)}
  template <suint_t Nd>
  constexpr num_t operator()(num_a<Nd> x, num_a<Nd> mu, num_t sigma) const
   noexcept {
    return gaussian(x, mu, num_a<Nd>::constant(sigma));
  }
};

namespace {
static constexpr auto&& gaussian = static_const<gaussian_fn>::value;
}  // namespace

/// Integrates a 1D function using the trapezoidal rule
///
/// \pre subdivisions > 2
struct integrate_trapezoidal_fn {
  template <typename F>
  constexpr num_t operator()(F&& f, num_t from, num_t to,
                             suint_t subdivisions) const noexcept {
    HM3_ASSERT(from < to,
               "domain-error: [from, to) = [{}, {}) (note: {} !< {})", from, to,
               from, to);
    HM3_ASSERT(subdivisions > 2, "subdivisions ({}) must be greater than 2",
               subdivisions);

    num_t dx = (to - from) / subdivisions;
    HM3_ASSERT(math::approx(from + dx * subdivisions, to), "");

    num_t value = f(from) / 2.;
    for (suint_t i = 1; i < subdivisions; ++i) {
      num_t x = from + i * dx;
      value += f(x);
    }

    value += f(to) / 2.;
    value *= dx;

    return value;
  }
};

namespace {
static constexpr auto&& integrate_trapezoidal
 = static_const<integrate_trapezoidal_fn>::value;
}  // namespace

/// Integrates a 1D function using the simpson rule
///
/// \pre subdivisions > 3 and subdivisions must be an even number
struct integrate_simpson_fn {
  template <typename F>
  constexpr num_t operator()(F&& f, num_t from, num_t to,
                             suint_t subdivisions) const noexcept {
    HM3_ASSERT(from < to,
               "domain-error: [from, to) = [{}, {}) (note: {} !< {})", from, to,
               from, to);

    num_t dx = (to - from) / subdivisions;
    HM3_ASSERT(math::approx(from + dx * subdivisions, to), "");
    HM3_ASSERT(subdivisions > 3, "subdivisions ({}) must be greater than 3",
               subdivisions);
    HM3_ASSERT(subdivisions % 2 == 0, "subdivisions ({}) must be even",
               subdivisions);

    num_t value = f(from);
    for (suint_t i = 1; i < subdivisions; i += 2) {
      value += 4. * f(from + i * dx);
      value += 2. * f(from + (i + 1) * dx);
    }

    value += f(to);
    value *= dx / 3.;

    return value;
  }
};

namespace {
static constexpr auto&& integrate_simpson
 = static_const<integrate_simpson_fn>::value;
}  // namespace

}  // namespace math
}  // namespace hm3
