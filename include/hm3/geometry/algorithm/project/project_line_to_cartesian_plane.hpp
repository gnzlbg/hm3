#pragma once
/// \file
///
/// Projects a line into a Cartesian plane.
#include <hm3/geometry/concept/line.hpp>
#include <hm3/geometry/concept/cartesian_plane.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry {

namespace project_line_to_cartesian_plane_detail {
struct project_line_to_cartesian_plane_fn {

  template <typename L, typename P,
            CONCEPT_REQUIRES_(Line<L, 3>{} and CartesianPlane<P, 3>{})>
  constexpr auto operator(L const &l, P const &p) const noexcept {
    const auto d = p.direction_idx();
    HM3_ASSERT(d < 3, "");
    constexpr dim_t is[3] = {1, 0, 0};
    constexpr dim_t js[3] = {2, 2, 1};
    dim_t i = is[d];
    dim_t j = js[d];
    using p_t = associated::point_t<L>;
    using p2d_t = trait::rebind_t<p_t, 2>;
    using v2d_t = associated::vector_t<p2d_t>;

    p2d_t o{l.origin()(i), l.origin()(j)};
    v2d_t v{l.direction()(i), l.direction()(j)};
    v = v().normalized();
    if (math::approx(v().norm(), 0.)) {
      return {};
    }
    using l2d_t = rebind_t<L, 2>;
    return l2d_t(o, v);
  }
};
} // namespace project_line_to_cartesian_plane_detail

namespace {
static constexpr auto const &project_line_to_cartesian_plane =
    static_const<project_line_to_cartesian_plane_detail::
                     project_line_to_cartesian_plane_fn>::value;
}

} // namespace hm3::geometry
