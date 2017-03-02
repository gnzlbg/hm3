#pragma once
/// \file
///
/// Relative position of point with respect to a line.
#include <hm3/geometry/algorithm/relative_position/point_line.hpp>

namespace hm3::geometry {

namespace relative_position_aabb_line_detail {
struct relative_position_aabb_line_fn {
  template <typename P, typename L,
            CONCEPT_REQUIRES_((Same<associated::t_<P>, trait::aabb<2>>{}
                               or Same<associated::t_<P>, trait::box<2>>{})
                              and Line<L, 2>{})>
  constexpr auto operator()(P const& b, L const& l) const noexcept {
    auto rp = relative_position_point_line(vertex(b, 0), l);
    for (dim_t vx = 1, e = vertex_size(b); vx != e; ++vx) {
      if (rp != relative_position_point_line(vertex(b, vx), l)) {
        return relative_position_t::intersected;
      }
    }
    return rp;
  }
};

}  // namespace relative_position_aabb_line_detail

namespace {
static constexpr auto const& relative_position_aabb_line = static_const<
 relative_position_aabb_line_detail::relative_position_aabb_line_fn>::value;
}  // namespace

}  // namespace hm3::geometry