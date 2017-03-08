#pragma once
/// \file
///
/// Relative position of point with respect to a line.
#include <hm3/geometry/algorithm/relative_position/point_line.hpp>

namespace hm3::geometry {

namespace relative_position_aabb_line_detail {
struct relative_position_aabb_line_fn {
  template <typename P, typename L>
  constexpr auto operator()(P const& b, L const& l, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(AABB<P, 2>{});
    static_assert(Line<L, 2>{});
    auto rp = relative_position_point_line(vertex(b, 0), l, abs_tol, rel_tol);
    for (dim_t vx = 1, e = vertex_size(b); vx != e; ++vx) {
      if (rp
          != relative_position_point_line(vertex(b, vx), l, abs_tol, rel_tol)) {
        return relative_position_t::intersected;
      }
    }
    return rp;
  }
};

}  // namespace relative_position_aabb_line_detail

namespace {
static constexpr auto const& relative_position_aabb_line
 = static_const<with_default_tolerance<
  relative_position_aabb_line_detail::relative_position_aabb_line_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
