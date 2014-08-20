#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Type traits
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include <type_traits>
////////////////////////////////////////////////////////////////////////////////
namespace hom3 {

/// Non-standard type traits and utilities
namespace traits {

namespace detail {
enum class enabler {};
}
constexpr detail::enabler dummy = {};

template <class C, class T = detail::enabler>
using enable_if = std::enable_if_t<C::value, T>;

template <class C, class T = detail::enabler>
using disable_if = std::enable_if_t<!(C::value), T>;

template <class T, T a, T b, class C = T>
struct equal : std::integral_constant<bool, a == b> {};

struct lazy {};
struct eager {};

namespace detail {

template <class T>
std::true_type has_iterator_category_helper(typename T::iterator_category*){};

template <class T> std::false_type has_iterator_category_helper(...){};

template <class T>
struct has_iterator_category : decltype(has_iterator_category_helper<T>(0)) {};
}  // namespace detail

/// Detect if T is an iterator
template <class T>
struct is_iterator
 : std::integral_constant<bool, std::is_pointer<T>::value
                                || detail::has_iterator_category<T>::value> {};

}  // namespace traits

using namespace traits;

}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
