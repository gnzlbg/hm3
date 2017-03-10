#pragma once
/// \file
///
/// Direction of a vector
#include <hm3/geometry/concept/vector.hpp>

namespace hm3::geometry {

namespace direction_vector_detail {

struct direction_vector_fn {
  /// Direction of a vector (its the vector itself).
  template <typename T>
  constexpr decltype(auto) operator()(T&& t) const noexcept {
    return std::forward<T>(t);
  }

  /// Returns a new vector with its direction inverted (original vector with all
  /// components multiplied with -1).
  template <typename T>
  static constexpr T invert(T const& t) noexcept {
    static_assert(Vector<T>{});
    return T{-1. * t()};
    ;
  }
};

}  // namespace direction_vector_detail

namespace {
constexpr auto const& direction_vector
 = static_const<direction_vector_detail::direction_vector_fn>::value;
}  // namespace

}  // namespace hm3::geometry
