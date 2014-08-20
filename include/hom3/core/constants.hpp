#pragma once
////////////////////////////////////////////////////////////////////////////////
// \file Numeric constants
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include <boost/math/constants/constants.hpp>
#include <limits>
#include <hom3/core/types.hpp>
#include <hom3/core/traits.hpp>
#include <hom3/core/returns.hpp>
#include <hom3/core/matrix/eigen.hpp>
////////////////////////////////////////////////////////////////////////////////

namespace hom3 {

template <class T, class B> struct arithmetic;

/// Numeric constants
namespace constant {

namespace detail {

template <class T>
constexpr auto invalid_value(T) RETURNS(std::numeric_limits<T>::max());

constexpr Num invalid_value(Num) noexcept {
  static_assert(std::numeric_limits<Num>::has_quiet_NaN, "no NaN support!");
  return std::numeric_limits<Num>::quiet_NaN();
}

template <class T, class B>
constexpr auto invalid_value(arithmetic<T, B>)
 RETURNS(arithmetic<T, B>{invalid_value(T{})});

}  // namespace detail

/// Invalid object of type T
///
/// Useful to represent an empty value without using an optional, e.g.,
/// - no nghbr has been found
/// - node id does not exist
template <class T>
constexpr auto make_invalid(T = T{}) RETURNS(constant::detail::invalid_value(T{}));

/// Polymorphic invalid type
struct invalid {
  template <class T>
  constexpr operator T() const {
    return make_invalid<T>();
  };

  // template<class Other>
  // bool operator==(Other&& o) { return o == static_cast<Other>(*this); }
};

// template<class Other>
// bool operator==(Other&& o, invalid i) { return i == std::forward<Other>(o); }

/// Is value of \p o valid?
template <class T>
static constexpr auto is_valid(const T& o) RETURNS(o != T{invalid{}});

/// Epsilon
static const constexpr Num eps = std::numeric_limits<Num>::epsilon();

/// Pi
static const constexpr Num pi = boost::math::constants::pi<Num>();

/// Zero
template <class T, enable_if<traits::is_eigen_matrix<T>> = traits::dummy>
constexpr auto zero(T&&) RETURNS(T::Zero());

template <class T, disable_if<traits::is_eigen_matrix<T>> = traits::dummy>
constexpr auto zero(T&&) RETURNS(T{0});

/// Dimensions
template <class T> constexpr auto dimensions(T&& t) RETURNS(t.cols());

}  // namespace constant

namespace math {

using namespace constant;

}  // namespace math

static constexpr constant::invalid invalid{};

using constant::is_valid;

}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
