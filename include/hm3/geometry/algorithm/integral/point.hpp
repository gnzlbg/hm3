#pragma once
/// \file
///
/// Integral of a Point.

#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace integral_point_detail {

struct integral_point_fn {
  template <typename T>
  constexpr associated::num_type_t<T> operator()(T&&,
                                            trait::surface_integral<T>) const {
    static_assert(Point<uncvref_t<T>, 1>{});
    return 1.;
  }
};

}  // namespace integral_point_detail

namespace {
static constexpr auto const& integral_point
 = static_const<integral_point_detail::integral_point_fn>::value;
}  // namespace

}  // namespace hm3::geometry
