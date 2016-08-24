#pragma once
/// \file
///
/// Generic unwrap values
#include <hm3/utility/compact_optional.hpp>

namespace hm3 {

template <typename T>
constexpr decltype(auto) unwrap(T&& t) noexcept {
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
