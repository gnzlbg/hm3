#pragma once
/// \file
///
/// Bounding volumes of an AABB.
#include <hm3/geometry/algorithm/bounding_volume.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
#include <hm3/geometry/primitive/aabb/bounding_length.hpp>
#include <hm3/geometry/primitive/box/box.hpp>

namespace hm3::geometry::aabb_primitive {

/// Axis-aligned bounding box of \p b.
template <typename T, typename UT = uncvref_t<T>, dim_t Nd = UT::dimension(),
          CONCEPT_REQUIRES_(Same<UT, aabb<Nd>>{})>
constexpr decltype(auto) axis_aligned_bounding_box(T&& b) noexcept {
  return b;
}

/// Axis-aligned bounding box of \p b.
template <typename T, typename UT = uncvref_t<T>, dim_t Nd = UT::dimension(),
          CONCEPT_REQUIRES_(Same<UT, aabb<Nd>>{})>
constexpr box<Nd> square_bounding_box(T&& b) noexcept {
  auto bbox = box<Nd>(centroid(b), max_bounding_length(b));

  HM3_ASSERT(centroid(bbox) == centroid(b), "");

  for (dim_t d = 0; d < Nd; ++d) {
    if (x_min(bbox)(d) > x_min(b)(d) or x_max(bbox)(d) < x_max(b)(d)) {
      num_t diff = std::max(std::abs(x_min(bbox)(d) - x_min(b)(d)),
                            std::abs(x_max(bbox)(d) - x_max(b)(d)));
      bbox = box<Nd>(centroid(b), max_bounding_length(b) + 2. * diff);
    }
  }

  HM3_ASSERT(
   x_min(bbox) <= x_min(b) and x_max(bbox) >= x_max(b),
   "the square bounding box {} of the aabb {} does not contain the aabb", bbox,
   b);
  return bbox;
}

}  // namespace hm3::geometry::aabb_primitive
