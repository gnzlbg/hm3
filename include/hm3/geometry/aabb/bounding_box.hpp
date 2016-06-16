/// \file
///
/// AABB bounding box
#include <hm3/geometry/aabb/aabb.hpp>
#include <hm3/geometry/aabb/centroid.hpp>
#include <hm3/geometry/aabb/length.hpp>
#include <hm3/geometry/box/box.hpp>

namespace hm3 {
namespace geometry {

/// Box of \p b
template <dim_t Nd>
constexpr box<Nd> square_bounding_box(aabb<Nd> aab) noexcept {
  return box<Nd>(centroid(aab), max_length(aab));
}

/// Axis-aligned bounding box of \p b
template <dim_t Nd>
constexpr aabb<Nd> axis_aligned_bounding_box(aabb<Nd> b) noexcept {
  return b;
}

}  // namespace geometry
}  // namespace hm3
