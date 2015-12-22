#pragma once
/// \file
///
/// Checks order of points in rectangles (not necesarily square)
#include <hm3/types.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/range.hpp>
#include <hm3/utility/fatal_error.hpp>

namespace hm3 {
namespace geometry {

template <typename Points, CONCEPT_REQUIRES_(RandomAccessRange<Points>{})>
bool are_rectangle_points_in_ccw_order(Points&& ps) {
  using point_t                = decltype(*begin(ps));
  static constexpr uint_t nd   = dimension(point_t{});
  static constexpr uint_t nreq = math::ipow(2_u, nd);

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
bool are_rectangle_points_in_z_order(Points&& ps) {
  using point_t                = decltype(*begin(ps));
  static constexpr uint_t nd   = dimension(point_t{});
  static constexpr uint_t nreq = math::ipow(2_u, nd);

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

}  // namespace geometry
}  // namespace hm3
