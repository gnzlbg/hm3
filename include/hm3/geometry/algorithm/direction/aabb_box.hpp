#pragma once
/// \file
///
/// Direction of a segment
#include <hm3/geometry/algorithm/bounding_length.hpp>
#include <hm3/geometry/concept/segment.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace direction_aabb_box_detail {

struct direction_aabb_box_fn {
  /// Direction of the line segment \p s (the resulting vector is normalized).
  template <typename T>
  constexpr decltype(auto) operator()(T&& s) const noexcept {
    static_assert(ConvertibleTo<associated::t_<T>, trait::aabb<1>>{});
    using v_t = associated::vector_t<T>;
    return v_t{bounding_length.max(s)};
  }
};

}  // namespace direction_aabb_box_detail

namespace {
constexpr auto const& direction_aabb_box
 = static_const<direction_aabb_box_detail::direction_aabb_box_fn>::value;
}  // namespace

}  // namespace hm3::geometry
