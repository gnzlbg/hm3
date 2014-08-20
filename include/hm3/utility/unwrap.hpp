#pragma once
/// \file primitive_cast.hpp Some functions need to work on values and value
/// wrappers as if they were values. Unwrap does this.
#include <hm3/utility/compact_optional.hpp>
#include <hm3/utility/requires.hpp>

namespace hm3 {

template <typename T> constexpr decltype(auto) unwrap(T&& t) noexcept {
  return std::forward<T>(t);
}

template <typename N, typename T>
constexpr decltype(auto) unwrap(compact_optional<N, T>&& t) noexcept {
  return *t;
}

template <typename N, typename T>
constexpr decltype(auto) unwrap(compact_optional<N, T>& t) noexcept {
  return *t;
}

template <typename N, typename T>
constexpr decltype(auto) unwrap(compact_optional<N, T> const& t) noexcept {
  return *t;
}

}  // namespace hm3
