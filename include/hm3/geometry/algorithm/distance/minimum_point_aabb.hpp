#pragma once
/// \file
///
/// Minimum distance between a point and an axis-aligned-bounding-box.
#include <hm3/geometry/algorithm/bounding_length/aabb.hpp>
#include <hm3/geometry/algorithm/centroid/aabb.hpp>
#include <hm3/geometry/concept/aabb.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/sd/functions/aabb.hpp>

namespace hm3::geometry {

namespace minimum_distance_point_aabb_detail {

struct minimum_distance_point_aabb_fn {
  /// Minimum distance between the point \p p and the aabb \p a.
  ///
  /// It computes the signed-distance of the point to the aabb and returns its
  /// magnitude.
  template <typename P, typename A, CONCEPT_REQUIRES_(Point<P>{} and AABB<A>{})>
  constexpr associated::num_type_t<P> operator()(P const& p, A const& a) const
   noexcept {
    return std::abs(sd::aabb_at_point<ad_v<P>>(centroid_aabb(a),
                                               bounding_length_aabb.all(a))(p));
  }
};

}  // namespace minimum_distance_point_aabb_detail

namespace {
static constexpr auto const& minimum_distance_point_aabb = static_const<
 minimum_distance_point_aabb_detail::minimum_distance_point_aabb_fn>::value;
}

}  // namespace hm3::geometry
