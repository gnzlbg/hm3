#pragma once
/// \file
///
/// Normal of a vector
#include <hm3/geometry/concept/vector.hpp>

namespace hm3::geometry {

namespace normal_vector_detail {

/// TODO: move this to a rotate algorithm?

/// Rotate the vector 90 degrees counter-clockwise
template <typename V, typename UV = uncvref_t<V>>
constexpr UV rotate_90_ccw(V const& v) noexcept {
  static_assert(Vector<UV, 2>{});
  return UV{-v(1), v(0)};
}

/// Rotate the vector 90 degrees clockwise
template <typename V, typename UV = uncvref_t<V>>
constexpr UV rotate_90_cw(V const& v) noexcept {
  static_assert(Vector<UV, 2>{});
  return UV{v(1), -v(0)};
}

struct normal_vector_fn {
  /// Normal of a 2D vector
  template <typename T>
  constexpr auto operator()(T&& t) const noexcept {
    static_assert(Vector<uncvref_t<T>, 2>{});
    return rotate_90_ccw(t);
  }
};

}  // namespace normal_vector_detail

namespace {
constexpr auto const& normal_vector
 = static_const<normal_vector_detail::normal_vector_fn>::value;
}  // namespace

}  // namespace hm3::geometry
