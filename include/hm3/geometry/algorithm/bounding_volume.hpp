#pragma once
/// \file
///
/// Bounding volume.
#include <hm3/geometry/algorithm/x_min_max.hpp>
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace bounding_volume_detail {

struct bounding_volume_fn {
  using self = bounding_volume_fn;
  /// Axis-Aligned Bounding Box
  template <typename T>
  static constexpr auto aabb(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   axis_aligned_bounding_box(std::forward<T>(t)));

  /// Square Bounding Box
  template <typename T>
  static constexpr auto box(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   square_bounding_box(std::forward<T>(t)));

  /// Axis-Aligned Bounding Box of a range of primitives.
  template <typename Rng, typename Prim = uncvref_t<ranges::range_value_t<Rng>>,
            dim_t Nd = Prim::dimension(),
            CONCEPT_REQUIRES_(!Primitive<Rng>{} and Range<Rng>{})>
  static constexpr auto aabb(Rng&& rng) -> geometry::aabb<Nd> {
    auto r_min = point<Nd>::constant(math::highest<num_t>);
    auto r_max = point<Nd>::constant(math::lowest<num_t>);
    for (auto&& o : rng) {
      // compute aabb of each element of the range:
      auto bb = self::aabb(o);
      // update aabb for all elements:
      for (dim_t d = 0; d < Nd; ++d) {
        r_min(d) = std::min(r_min(d), x_min(bb)(d));
        r_max(d) = std::max(r_max(d), x_max(bb)(d));
      }
    }
    return geometry::aabb<Nd>(r_min, r_max);
  }
};

}  // namespace bounding_volume_detail

namespace {
static constexpr auto const& bounding_volume
 = static_const<bounding_volume_detail::bounding_volume_fn>::value;
}

}  // namespace hm3::geometry
