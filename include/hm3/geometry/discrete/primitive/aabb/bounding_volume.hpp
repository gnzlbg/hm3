#pragma once
/// \file
///
/// Bounding volumes of an AABB.
#include <hm3/geometry/discrete/algorithm/bounding_volume.hpp>
#include <hm3/geometry/discrete/primitive/aabb/aabb.hpp>
#include <hm3/geometry/discrete/primitive/aabb/bounding_length.hpp>
#include <hm3/geometry/discrete/primitive/box/box.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace aabb_primitive {

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
  return box<Nd>(centroid(b), max_bounding_length(b));
}

}  // namespace aabb_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
