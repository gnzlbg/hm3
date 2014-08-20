#pragma once
/// \file floating_point.hpp Floating-point arithmetic utilities
#include <limits>
#include <type_traits>

namespace hm3 {
namespace math {
namespace detail {

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
/// size to type.  It maps a size in bytes to a primitive type with that
/// size. e.g.
///
/// \code
///   TypeWithSize<4>::UInt
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
template <std::size_t size> struct TypeWithSize {
  /// This prevents the user from using TypeWithSize<N> with incorrect
  /// values of N.
  using UInt = void;
};

/// The specialization for size 4.
template <> struct TypeWithSize<4> {
  /// unsigned int has size 4 in both gcc and MSVC.
  ///
  /// As base/basictypes.h doesn't compile on Windows, we cannot use
  /// uint32, uint64, and etc here.
  using Int  = int;
  using UInt = unsigned int;
};

/// The specialization for size 8.
template <> struct TypeWithSize<8> {
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
template <typename RawType> struct FloatingPoint {
  /// Defines the unsigned integer type that has the same size as the
  /// floating point number.
  using Bits = typename TypeWithSize<sizeof(RawType)>::UInt;

  /// Constants.

  /// # of bits in a number.
  static const constexpr std::size_t kBitCount = 8 * sizeof(RawType);

  /// # of fraction bits in a number.
  static const constexpr std::size_t kFractionBitCount
   = std::numeric_limits<RawType>::digits - 1;

  /// # of exponent bits in a number.
  static const constexpr std::size_t kExponentBitCount
   = kBitCount - 1 - kFractionBitCount;

  /// The mask for the sign bit.
  static const constexpr Bits kSignBitMask = static_cast<Bits>(1)
                                             << (kBitCount - 1);

  // The mask for the fraction bits.
  static const constexpr Bits kFractionBitMask
   = (~static_cast<Bits>(0) >> (kExponentBitCount + 1));

  /// The mask for the exponent bits.
  static const constexpr Bits kExponentBitMask
   = ~(kSignBitMask | kFractionBitMask);

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
  static const constexpr std::size_t kMaxUlps = 4;

  /// Constructs a FloatingPoint from a raw floating-point number.
  ///
  /// On an Intel CPU, passing a non-normalized NAN (Not a Number)
  /// around may change its bits, although the new value is guaranteed
  /// to be also a NAN.  Therefore, don't expect this constructor to
  /// preserve the bits in x when x is a NAN.
  constexpr explicit FloatingPoint(const RawType& x) noexcept : u_{x} {}

 private:
  constexpr explicit FloatingPoint(const Bits& b) noexcept : u_{b} {}

 public:
  /// Static methods

  /// Reinterprets a bit pattern as a floating-point number.
  ///
  /// This function is needed to test the AlmostEquals() method.
  static constexpr RawType ReinterpretBits(const Bits bits) noexcept {
    FloatingPoint fp(bits);
    return fp.u_.value_;
  }

  /// Returns the floating-point number that represent positive infinity.
  static constexpr RawType Infinity() noexcept {
    return ReinterpretBits(kExponentBitMask);
  }

  /// Non-static methods

  /// Returns the bits that represents this number.
  constexpr const Bits& bits() const noexcept { return u_.bits_; }

  /// Returns the exponent bits of this number.
  constexpr Bits exponent_bits() const noexcept {
    return kExponentBitMask & u_.bits_;
  }

  /// Returns the fraction bits of this number.
  constexpr Bits fraction_bits() const noexcept {
    return kFractionBitMask & u_.bits_;
  }

  /// Returns the sign bit of this number.
  constexpr Bits sign_bit() const noexcept { return kSignBitMask & u_.bits_; }

  /// Returns true iff this is NAN (not a number).
  constexpr bool is_nan() const noexcept {
    /// It's a NAN if the exponent bits are all ones and the fraction
    /// bits are not entirely zeros.
    return (exponent_bits() == kExponentBitMask) && (fraction_bits() != 0);
  }

  /// Returns true iff this number is at most kMaxUlps ULP's away from
  /// rhs.  In particular, this function:
  ///
  ///   - returns false if either number is (or both are) NAN.
  ///   - treats really large numbers as almost equal to infinity.
  ///   - thinks +0.0 and -0.0 are 0 DLP's apart.
  constexpr bool AlmostEquals(const FloatingPoint& rhs) const noexcept {
    /// The IEEE standard says that any comparison operation involving
    /// a NAN must return false.
    if (is_nan() || rhs.is_nan()) { return false; }

    return DistanceBetweenSignAndMagnitudeNumbers(u_.bits_, rhs.u_.bits_)
           <= kMaxUlps;
  }

 private:
  /// The data type used to store the actual floating-point number.
  union FloatingPointUnion {
    constexpr FloatingPointUnion(RawType val) : value_{val} {}
    constexpr FloatingPointUnion(Bits bits) : bits_{bits} {}
    RawType value_;  ///< The raw floating-point number.
    Bits bits_;      ///< The bits that represent the number.
  };

  /// Converts an integer from the sign-and-magnitude representation to
  /// the biased representation.  More precisely, let N be 2 to the
  /// power of (kBitCount - 1), an integer x is represented by the
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
  constexpr static Bits SignAndMagnitudeToBiased(const Bits& sam) noexcept {
    if (kSignBitMask & sam) {
      /// sam represents a negative number.
      return ~sam + 1;
    } else {
      /// sam represents a positive number.
      return kSignBitMask | sam;
    }
  }

  /// Given two numbers in the sign-and-magnitude representation,
  /// returns the distance between them as an unsigned number.
  constexpr static Bits DistanceBetweenSignAndMagnitudeNumbers(
   const Bits& sam1, const Bits& sam2) noexcept {
    const Bits biased1 = SignAndMagnitudeToBiased(sam1);
    const Bits biased2 = SignAndMagnitudeToBiased(sam2);
    return (biased1 >= biased2) ? (biased1 - biased2) : (biased2 - biased1);
  }

  FloatingPointUnion u_;
};

/// Creates a floating point type
///
/// Note: this is an alias for the GoogleTest FPA class
///
/// Example:
/// \code
/// if ( floating_point(a) == floating_point(b) ) {...}
/// \endcode
///
template <class T> using floating_point = FloatingPoint<T>;

}  // namespace detail

/// Compares two floating point values for equality
template <class T, class U>
static constexpr bool approx(const T& a, const U& b) noexcept {
  static_assert(std::is_floating_point<T>::value
                 || std::is_floating_point<U>::value,
                "requires a FloatingPoint type");

  using fp_type
   = std::conditional_t<std::is_floating_point<T>::value
                         && std::is_floating_point<U>::value,
                        decltype(std::declval<T>() * std::declval<U>()),
                        std::conditional_t<std::is_floating_point<T>::value, T,
                                           void>>;
  static_assert(!std::is_same<fp_type, void>::value, "unexpected fp type!");

  return detail::floating_point<fp_type>{static_cast<fp_type>(a)}.AlmostEquals(
   detail::floating_point<fp_type>{static_cast<fp_type>(b)});
}

}  // namespace math
}  // namespace hm3
