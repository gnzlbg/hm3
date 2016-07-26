#pragma once
/// \file
///
/// Bounding volumes of an AABB.
#include <hm3/geometry/discrete/algorithm/bounding_volume.hpp>
#include <hm3/geometry/discrete/primitive/aabb/aabb.hpp>
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
  return box<Nd>(std::forward<T>(b));
}

/// Axis-aligned bounding box of a range of AABBs \p rng.
template <typename Rng, typename AABB = uncvref_t<ranges::range_value_t<Rng>>,
          dim_t Nd = AABB::dimension(),
          CONCEPT_REQUIRES_(Range<Rng>{} and Same<AABB, aabb<Nd>>{})>
constexpr aabb<Nd> axis_aligned_bounding_box(Rng&& rng) noexcept {
  auto r_min = point<Nd>::constant(math::highest<num_t>);
  auto r_max = point<Nd>::constant(math::lowest<num_t>);

  for (auto&& box : rng) {
    auto bmin = x_min(box);
    auto bmax = x_max(box);
    for (dim_t d = 0; d < Nd; ++d) {
      r_min(d) = std::min(r_min(d), bmin(d));
      r_max(d) = std::max(r_max(d), bmax(d));
    }
  }
  return aabb<Nd>(r_min, r_max);
}

}  // namespace aabb_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
