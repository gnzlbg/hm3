#pragma once
/// \file
///
/// Distance betweem the centroids of two geometric objects.
#include <hm3/geometry/algorithm/centroid.hpp>
#include <hm3/geometry/algorithm/distance/minimum_point_point.hpp>

namespace hm3::geometry {

namespace distance_centroid_detail {

struct distance_centroid_fn {
  /// Minimum distance between the geometric object \p t and \t u.
  template <typename T, typename U>
  constexpr auto operator()(T const& t, U const& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    minimum_distance_point_point(centroid(t), centroid(u)));
};

}  // namespace distance_centroid_detail

namespace {
static constexpr auto const& distance_centroid
 = static_const<distance_centroid_detail::distance_centroid_fn>::value;
}

}  // namespace hm3::geometry
