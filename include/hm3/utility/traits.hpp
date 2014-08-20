
#pragma once
/// \file traits.hpp Type traits
#include <type_traits>
#include <hm3/utility/meta.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {

template <class T> using uncvref_t = ranges::uncvref_t<T>;

/// Non-standard type traits and utilities
namespace traits {

// template <class T, T a, T b, class C = T>
// struct equal : std::integral_constant<bool, a == b> {};

struct lazy {};
struct eager {};

template <class T> using is_evaluation_t = meta::in<meta::list<lazy, eager>, T>;

}  // namespace traits

}  // namespace hm3
