#pragma once
// \file constants.hpp Numeric constants
#include <limits>

namespace hm3 {
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

}  // namespace math
}  // namespace hm3
