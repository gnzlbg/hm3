#pragma once
/// \file
///
/// Minimum distance between a plane and a point.
#include <hm3/geometry/concept/plane.hpp>
#include <hm3/geometry/concept/point.hpp>

namespace hm3::geometry {

namespace minimum_distance_plane_point_detail {

struct minimum_distance_plane_point_fn {
  /// Minimum distance between the plane \p l and the point \p p.
  template <typename L, typename P,
            CONCEPT_REQUIRES_(Plane<L>{} and Point<P>{})>
  constexpr auto operator()(L const& l, P const& p) const noexcept {
    return l.abs_distance(p);
  }
};

}  // namespace minimum_distance_plane_point_detail

namespace {
constexpr auto const& minimum_distance_plane_point = static_const<
 minimum_distance_plane_point_detail::minimum_distance_plane_point_fn>::value;
}

}  // namespace hm3::geometry
