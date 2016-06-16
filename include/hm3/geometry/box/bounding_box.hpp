/// \file
///
/// Box bounding box
#include <hm3/geometry/aabb/aabb.hpp>
#include <hm3/geometry/box/box.hpp>
#include <hm3/geometry/box/x_min_max.hpp>

namespace hm3 {
namespace geometry {

/// Box of \p b
template <dim_t Nd> constexpr box<Nd> square_bounding_box(box<Nd> b) noexcept {
  return b;
}

/// Axis-aligned bounding box of \p b
template <dim_t Nd>
constexpr aabb<Nd> axis_aligned_bounding_box(box<Nd> b) noexcept {
  return aabb<Nd>(x_min(b), x_max(b));
}

}  // namespace geometry
}  // namespace hm3
