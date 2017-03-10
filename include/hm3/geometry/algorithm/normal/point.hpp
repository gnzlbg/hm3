#pragma once
/// \file
///
/// Normal of a point
#include <hm3/geometry/concept/point.hpp>

namespace hm3::geometry {

namespace normal_point_detail {

struct normal_point_fn {
  /// Normal of a 1D point
  template <typename T>
  constexpr auto operator()(T&&) const noexcept {
    static_assert(Point<uncvref_t<T>, 1>{});
    return associated::vector_t<T>::constant(1.);
  }
};

}  // namespace normal_point_detail

namespace {
constexpr auto const& normal_point
 = static_const<normal_point_detail::normal_point_fn>::value;
}  // namespace

}  // namespace hm3::geometry
