#pragma once
// \file constants.hpp Numeric constants
#include <limits>
#include <hm3/types.hpp>
#include <hm3/utility/fwd.hpp>
#include <hm3/utility/traits.hpp>
#include <hm3/utility/returns.hpp>
#include <hm3/utility/matrix/eigen.hpp>

namespace hm3 {

/// Numeric constants
namespace constant {

namespace detail {

template <class T>
constexpr auto invalid_value(T) HM3_RETURNS(std::numeric_limits<T>::max());

constexpr num_t invalid_value(num_t) noexcept {
  static_assert(std::numeric_limits<num_t>::has_quiet_NaN, "no NaN support!");
  return std::numeric_limits<num_t>::quiet_NaN();
}

template <class T, class B>
constexpr auto invalid_value(arithmetic<T, B>)
 HM3_RETURNS(arithmetic<T, B>{invalid_value(T{})});

}  // namespace detail

/// Invalid object of type T
///
/// Useful to represent an empty value without using an optional, e.g.,
/// - no nghbr has been found
/// - node id does not exist
template <class T>
constexpr auto make_invalid(T = T{})
 HM3_RETURNS(constant::detail::invalid_value(T{}));

/// Polymorphic invalid type
struct invalid {
  template <class T> constexpr operator T() const { return make_invalid<T>(); };
};

/// Is value of \p o valid?
template <class T>
static constexpr auto is_valid(const T& o) HM3_RETURNS(o != T{invalid{}});

/// Epsilon
static const constexpr num_t eps = std::numeric_limits<num_t>::epsilon();

/// Pi
static const constexpr num_t pi
 = 3.141592653589793238462643383279502884197169399375105820974944;

/// E
static const constexpr num_t e
 = 2.7182818284590452353602874713526624977572470936999595749669676;

/// Zero
template <class T, CONCEPT_REQUIRES_(traits::is_eigen_matrix<T>{})>
constexpr auto zero(T&&) HM3_RETURNS(T::Zero());

template <class T, CONCEPT_REQUIRES_(!traits::is_eigen_matrix<T>{})>
constexpr auto zero(T&&) HM3_RETURNS(T{0});

/// Dimensions
template <class T> constexpr auto dimensions(T&& t) HM3_RETURNS(t.cols());

}  // namespace constant

static constexpr constant::invalid invalid{};

using constant::is_valid;

}  // namespace hm3
