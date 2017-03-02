#pragma once
/// \file
///
/// Direction of a line
#include <hm3/geometry/algorithm/direction/vector.hpp>
#include <hm3/geometry/concept/line.hpp>
#include <hm3/geometry/concept/ray.hpp>

namespace hm3::geometry {

namespace direction_line_detail {

struct direction_line_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const {
    static_assert(LineOrRay<T>{});
    return t.direction();
  }

  template <typename T, typename UT = uncvref_t<T>>
  static constexpr UT invert(T&& t) {
    static_assert(LineOrRay<UT>{});
    return UT(t.origin(), direction_vector.invert(direction_line_fn{}(t)));
  }
};

}  // namespace direction_line_detail

namespace {
static constexpr auto const& direction_line
 = static_const<direction_line_detail::direction_line_fn>::value;
}  // namespace

}  // namespace hm3::geometry
