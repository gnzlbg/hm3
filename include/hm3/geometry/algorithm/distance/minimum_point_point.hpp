#pragma once
/// \file
///
/// Minimum distance between two points.
#include <hm3/geometry/concept/point.hpp>

namespace hm3::geometry {

namespace minimum_distance_point_point_detail {

struct minimum_distance_point_point_fn {
  /// Minimum distance between the points \p p0 and \p p1.
  template <typename P0, typename P1,
            CONCEPT_REQUIRES_(Point<P0>{} and Point<P1>{})>
  constexpr associated::num_type_t<P0> operator()(P0 const& p0,
                                                  P1 const& p1) const noexcept {
    return (p0() - p1()).norm();
  }
};

}  // namespace minimum_distance_point_point_detail

namespace {
static constexpr auto const& minimum_distance_point_point = static_const<
 minimum_distance_point_point_detail::minimum_distance_point_point_fn>::value;
}

}  // namespace hm3::geometry
