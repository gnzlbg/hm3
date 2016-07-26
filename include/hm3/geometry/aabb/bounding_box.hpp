/// \file
///
/// AABB bounding box
#include <hm3/geometry/aabb/aabb.hpp>
#include <hm3/geometry/aabb/centroid.hpp>
#include <hm3/geometry/aabb/length.hpp>
#include <hm3/geometry/box/box.hpp>

namespace hm3 {
namespace geometry {

namespace aabb_primitive {

/// Box of \p b.
template <dim_t Nd>
constexpr box<Nd> square_bounding_box(aabb<Nd> aab) noexcept {
  return box<Nd>(centroid(aab), max_length(aab));
}

/// Axis-aligned bounding box of \p b.
template <dim_t Nd>
constexpr aabb<Nd> axis_aligned_bounding_box(aabb<Nd> b) noexcept {
  return b;
}



/// Axis-aligned bounding box of a range of AABBs \p rng.
template <typename Rng, typename AABB = uncvref_t<ranges::range_value_t<Rng>>,
          dim_t Nd = AABB::dimension(),
          CONCEPT_REQUIRES_(Range<Rng>{} and std::is_same<AABB, aabb<Nd>>{})>
constexpr aabb<Nd> axis_aligned_bounding_box(Rng&& rng) noexcept {
  auto r_min = point<Nd>::constant(math::highest<num_t>);
  auto r_max = point<Nd>::constant(math::lowest<num_t>);

  for (auto&& box : rng) {
    auto bs = bounds(box);
    for (dim_t d = 0; d < Nd; ++d) {
      r_min(d) = std::min(r_min(d), bs.min(d));
      r_max(d) = std::max(r_max(d), bs.max(d));
    }
  }
  return aabb<Nd>(r_min, r_max);
}

}  // namespace aabb_primitive

}  // namespace geometry
}  // namespace hm3
