#pragma once
/// \file
///
/// Bounding volumes of an box.
#include <hm3/geometry/algorithm/bounding_volume.hpp>
#include <hm3/geometry/primitive/box/box.hpp>

namespace hm3::geometry::box_primitive {

/// Square bounding box of \p b.
template <typename T, typename UT = uncvref_t<T>, dim_t Nd = UT::dimension(),
          CONCEPT_REQUIRES_(Same<UT, box<Nd>>{})>
constexpr decltype(auto) square_bounding_box(T&& b) noexcept {
  return b;
}

/// Axis-aligned bounding box of \p b.
template <typename T, typename UT = uncvref_t<T>, dim_t Nd = UT::dimension(),
          CONCEPT_REQUIRES_(Same<UT, box<Nd>>{})>
constexpr aabb<Nd> axis_aligned_bounding_box(T&& b) noexcept {
  return aabb<Nd>(x_min(b), x_max(b));
}

/// Axis-aligned bounding box of a range of boxes \p rng.
///
/// \todo Not implemented.
template <typename Rng, typename Box = uncvref_t<ranges::range_value_t<Rng>>,
          dim_t Nd = Box::dimension(),
          CONCEPT_REQUIRES_(Range<Rng>{} and Same<Box, box<Nd>>{})>
constexpr aabb<Nd> axis_aligned_bounding_box(Rng&& rng) noexcept = delete;

}  // namespace hm3::geometry::box_primitive
