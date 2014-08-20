#pragma once
/// \file meta.hpp Metaprogramming utilities
#include <functional>
#include <meta/meta.hpp>

namespace meta {

namespace detail {
template <typename T, typename BinaryFunction> struct folder_c {
  constexpr folder_c(T& acc, BinaryFunction& bf) : acc_{acc}, bf_{bf} {}
  T& acc_;
  BinaryFunction& bf_;
  constexpr void operator()(T const& i) const { acc_ = bf_(acc_, i); }
};
}  // namespace detail

/// Left-associative fold of an integer sequence using the binary function \p bf
/// and start value \p start.
template <class T, T... Ts, class BinaryFunction = std::plus<>>
constexpr auto fold_c(meta::integer_sequence<T, Ts...>&&, T start = T{},
                      BinaryFunction&& bf = BinaryFunction{}) -> T {
  meta::for_each(meta::as_list<meta::integer_sequence<T, Ts...>>{},
                 detail::folder_c<T, BinaryFunction>{start, bf});
  return start;
}

}  // namespace meta
