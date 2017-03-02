#pragma once
/// \file
///
/// Mathematical constants.
#include <hm3/types.hpp>
#include <limits>

namespace hm3::math {

/// Numeric constants
namespace constant {

/// Epsilon
template <typename T>
static const constexpr T eps_v = std::numeric_limits<T>::epsilon();

/// Pi
template <typename T>
static const constexpr T pi_v
 = 3.141592653589793238462643383279502884197169399375105820974944;

/// Euler's number
template <typename T>
static const constexpr T e_v
 = 2.7182818284590452353602874713526624977572470936999595749669676;

/// Infinity
template <typename T>
static const constexpr T inf_v = std::numeric_limits<T>::infinity();

static const constexpr num_t eps = eps_v<num_t>;
static const constexpr num_t pi  = pi_v<num_t>;
static const constexpr num_t e   = e_v<num_t>;
static const constexpr num_t inf = inf_v<num_t>;

template <typename T>
static const constexpr T highest = std::numeric_limits<T>::max();

template <typename T>
static const constexpr T lowest = std::numeric_limits<T>::lowest();

}  // namespace constant

using constant::pi;
using constant::e;
using constant::eps;
using constant::inf;
using constant::lowest;
using constant::highest;

}  // namespace hm3::math
