#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Integral constant
////////////////////////////////////////////////////////////////////////////////
#include <functional>
#include <type_traits>
#include <hom3/core/returns.hpp>

namespace hom3 {

namespace integral {

/// Integral wrapper
///
/// \tparam T type of the integral
/// \tparam value value of the integral
template <class T, T value> using t = std::integral_constant<T, value>;

/// Integral range wrapper [from,to).
///
/// \tparam T type of integrals in the range
/// \tparam from beginning of the integral range
/// \tparam to one past the end of the integral range
template <class T, T from, T to> struct range_c {
  static_assert(from <= to, "[from, to) requires from <= to");
};

/// Applies \param uf for each element in the integral range [from,to).
///
/// \tparam T type of integrals in the range
/// \tparam from beginning of the integral range
/// \tparam to one past the end of the integral range
/// \tparam UnaryFunction unary-function f(V) where V is implicitly convertible
///                       from T.
template <class T, T from, T to, class UnaryFunction>
constexpr void for_each(range_c<T, from, to>&&, UnaryFunction&& uf) {
  static_assert(from <= to, "invalid range");
  for (T i = from; i != to; ++i) { uf(i); }
}

/// Left-associative fold of an integral range [from,to) using \p bf and a
/// \param start value.
///
/// \tparam T type of integrals in the range
/// \tparam from beginning of the integral range
/// \tparam to one past the end of the integral range
/// \tparam BinaryFunction binary-function f(V1,V2) where V1 and V2 are
///         implicitly convertible from T.
///
/// \todo the start value should have type V1.
template <class T, T from, T to, class BinaryFunction = std::plus<>>
constexpr auto fold(range_c<T, from, to>&&, T start = T{},
                    BinaryFunction && bf = BinaryFunction{}) -> T {
  static_assert(from <= to, "invalid range");
  for (T i = from; i != to; ++i) { start = bf(start, i); }
  return start;
}

/// Heterogeneous type (stateless) list
///
/// \tparam Ts elements of the list
template <class... Ts> struct list {};

/// Is list empty?
template <class... Ts>
constexpr auto is_empty(list<Ts...>&&) RETURNS(t<bool, sizeof...(Ts) == 0>{});

/// Returns the head of the list x:xs -> x
///
/// \tparam Head head of the list
/// \tparam Ts tail of the list
template <class Head, class... Ts>
constexpr auto head(list<Head, Ts...>&&) RETURNS(Head{});

/// Returns the tail of the list x:xs -> xs
///
/// \tparam Head head of the list
/// \tparam Ts tail of the list
template <class Head, class... Ts>
constexpr auto tail(list<Head, Ts...>&&) RETURNS(list<Ts...>{});

namespace detail {

template <class UnaryFunction, class... Ts>
// constexpr auto for_each_(list<Ts...>, UnaryFunction&& f, std::true_type)
//     RETURNS(std::forward<UnaryFunction>(f));

constexpr void for_each_(list<Ts...>, UnaryFunction&&, std::true_type) {}

template <class UnaryFunction, class... Ts>
constexpr void for_each_(list<Ts...>, UnaryFunction&& f, std::false_type) {
  using h = decltype(head(list<Ts...>{}));
  using t = decltype(tail(list<Ts...>{}));
  f(h{});
  for_each_(t{}, std::forward<UnaryFunction>(f), is_empty(t{}));
}

}  // namespace detail

/// Applies \p uf to each element in the list \p l.
///
/// \tparam Ts elements of the list.
/// \tparam UnaryFunction unary-function(V) where V is implicitly convertible
/// from each element of the list.
template <class UnaryFunction, class... Ts>
constexpr void for_each(list<Ts...> l, UnaryFunction&& uf) {
  detail::for_each_(std::move(l), std::forward<UnaryFunction>(uf),
                    is_empty(list<Ts...>{}));
}

}  // namespace integral

}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
