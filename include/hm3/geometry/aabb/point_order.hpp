#pragma once
/// \file
///
/// Checks order of points in AABBs
#include <hm3/geometry/aabb/concept.hpp>
#include <hm3/geometry/polygon/types.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/config/fatal_error.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {

namespace aabb_primitive {

template <typename Points, CONCEPT_REQUIRES_(RandomAccessRange<Points>{})>
bool are_aabb_points_in_ccw_order(Points&& ps) {
  using point_t                 = decltype(*begin(ps));
  static constexpr dim_t nd     = dimension(point_t{});
  static constexpr ppidx_t nreq = math::ipow(ppidx_t{2}, ppidx_t{nd});

  auto&& np = size(ps);
  HM3_ASSERT(np == nreq, "{} points required but {} provided", nreq, np);

  switch (nd) {
    case 1: {
      if (!(ps[0](0) < ps[1](0))) { return false; }
      return true;
    }
    case 2: {
      if (!(ps[0](0) < ps[1](0))) { return false; }
      if (!(ps[3](0) < ps[2](0))) { return false; }
      if (!(math::approx(ps[0](1), ps[1](1)))) { return false; }
      if (!(math::approx(ps[3](1), ps[2](1)))) { return false; }

      if (!(ps[0](1) < ps[3](1))) { return false; }
      if (!(ps[1](1) < ps[2](1))) { return false; }
      if (!(math::approx(ps[0](0), ps[3](0)))) { return false; }
      if (!(math::approx(ps[1](0), ps[2](0)))) { return false; }
      return true;
    }
    case 3: {
      HM3_FATAL_ERROR("to be implemented");
    }
    default: { HM3_FATAL_ERROR("unreachable"); }
  }
}

template <typename Points, CONCEPT_REQUIRES_(RandomAccessRange<Points>{})>
bool are_aabb_points_in_z_order(Points&& ps) {
  using point_t                 = decltype(*begin(ps));
  static constexpr dim_t nd     = dimension(point_t{});
  static constexpr suint_t nreq = math::ipow(suint_t{2}, suint_t{nd});

  auto&& np = size(ps);
  HM3_ASSERT(np == nreq, "{} points required but {} provided", nreq, np);

  switch (nd) {
    case 1: {
      if (!(ps[0](0) < ps[1](0))) { return false; }
      return true;
    }
    case 2: {
      if (!(ps[0](0) < ps[1](0))) { return false; }
      if (!(ps[2](0) < ps[3](0))) { return false; }
      if (!(math::approx(ps[0](1), ps[1](1)))) { return false; }
      if (!(math::approx(ps[2](1), ps[3](1)))) { return false; }

      if (!(ps[0](1) < ps[2](1))) { return false; }
      if (!(ps[1](1) < ps[3](1))) { return false; }
      if (!(math::approx(ps[0](0), ps[2](0)))) { return false; }
      if (!(math::approx(ps[1](0), ps[3](0)))) { return false; }
      return true;
    }
    case 3: {
      HM3_FATAL_ERROR("to be implemented");
    }
    default: { HM3_FATAL_ERROR("unreachable"); }
  }
}

}  // namespace aabb_primitive

}  // namespace geometry
}  // namespace hm3
