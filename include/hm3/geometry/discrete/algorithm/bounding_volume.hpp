#pragma once
/// \file
///
/// Bounding volume.
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace detail {

struct bounding_volume_fn {
  /// Axis-Aligned Bounding Box
  template <typename T>
  static constexpr auto aabb(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   axis_aligned_bounding_box(std::forward<T>(t)));

  /// Square Bounding Box
  template <typename T>
  static constexpr auto box(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   square_bounding_box(std::forward<T>(t)));
};

}  // namespace detail

namespace {
static constexpr auto const& bounding_volume
 = static_const<detail::bounding_volume_fn>::value;
}

}  // namespace discrete

using discrete::bounding_volume;

}  // namespace geometry
}  // namespace hm3
