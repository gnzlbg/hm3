#pragma once
/// \file
///
/// Unsigned arithmetic
///
/// TODO: move some functions from hm3/utility/bit.hpp to here
#include <hm3/utility/range.hpp>
#include <type_traits>

namespace hm3::math {

/// Takes the absolute difference between two unsigned integers (without
/// wrapping)
template <typename Int, CONCEPT_REQUIRES_(UnsignedIntegral<Int>{})>
static constexpr Int absdiff(Int a, Int b) noexcept {
  static_assert(std::is_unsigned<Int>::value,
                "requires an UnsignedIntegral type");
  return (a > b) ? (a - b) : (b - a);
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

}  // namespace hm3::math
