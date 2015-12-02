#pragma once
/// \file
#include <array>
#include <hm3/geometry/line.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/polygon.hpp>
#include <hm3/geometry/simplex.hpp>
#include <hm3/geometry/square.hpp>
#include <hm3/types.hpp>

namespace hm3 {
namespace geometry {

/// For a line \p l with edge values v, linearly interpolates \p v at position
/// \p d from l.x_0 in the direction of l.x_1
template <typename T, uint_t Nd>
T interpolate(line<Nd> l, std::array<T, 2> v, num_t x) {
  return v[0] + (v[1] - v[0]) / (l.x_1() - l.x_0()).norm() * x;
}

/// For a line \p l with edge values v, find the distance from l.x_0 to the
/// point with value \p val (positive in direction from x_0 to x_1)
template <typename T, uint_t Nd>
num_t distance_to_value(line<Nd> l, std::array<T, 2> v, T val) {
  return (l.x_1() - l.x_0()).norm() * (val - v[0]) / (v[1] - v[0]);
}

/// For a line \p l with edge values v, find the point on the line with value \p
/// val
template <typename T, uint_t Nd>
point<Nd> point_with_value(line<Nd> l, std::array<T, 2> v, T val) {
  point<Nd> tmp;
  auto dir = direction(l);
  tmp()    = l.x_0() + dir() * distance_to_value(l, v, val);
  // l().pointAt(distance_to_value(l, v, val));
  return tmp;
}

///
template <typename Shape, typename SignedDistance,
          typename ShapeT = std::decay_t<Shape>,
          CONCEPT_REQUIRES_(ShapeT::dimension() == 2_u)>
bool is_intersected(Shape&& s, SignedDistance sd) noexcept {
  constexpr suint_t no_corners = ShapeT::no_corners();
  auto c                       = corners(s);
  // Compute the sign of the values at the corners:
  bool neg_corner = false;
  bool pos_corner = false;
  for (suint_t i = 0; i != no_corners; ++i) {
    if (math::signum(sd(c[i])) == 1) { pos_corner = true; }
    if (math::signum(sd(c[i])) == -1) { neg_corner = true; }
  }
  // a cell is intersected if it has at least one positive and one negative
  // corner
  //
  // e.g. a cell with all positive corners except one zero corner is not
  // intersected
  return (pos_corner and neg_corner);
}

template <typename Shape, typename SignedDistance,
          typename ShapeT = std::decay_t<Shape>>
bool is_completely_inside(Shape&& s, SignedDistance sd) noexcept {
  return sd(geometry::center(s)) > 0. and !is_intersected(s, sd);
}

template <typename Shape, typename SignedDistance,
          typename ShapeT = std::decay_t<Shape>,
          CONCEPT_REQUIRES_(ShapeT::dimension() == 2_u)>
std::tuple<polygon<2, 5>, polygon<2, 5>> intersect(Shape&& s,
                                                   SignedDistance sd) noexcept {
  std::tuple<polygon<2, 5>, polygon<2, 5>> shapes;
  constexpr uint_t Nd          = ShapeT::dimension();
  constexpr suint_t no_corners = ShapeT::no_corners();
  auto shape_corners           = corners(s);
  std::array<num_t, no_corners> sd_values;
  stack::vector<int, 5> sg_values_p0;
  stack::vector<int, 5> sg_values_p1;

  // Compute signed distance values at the corners
  for (suint_t cidx = 0; cidx != no_corners; ++cidx) {
    sd_values[cidx] = sd(shape_corners[cidx]);
  }

  suint_t no_cutpoints_found = 0;
  /// Adds corners and cut points to each polygon
  for (suint_t cidx = 0; cidx != no_corners; ++cidx) {
    // std::cout << "shape0:" << std::get<0>(shapes) << std::endl;
    // std::cout << "shape1:" << std::get<1>(shapes) << std::endl;
    auto current_sd_value = sd_values[cidx];
    auto next_corner   = cidx != no_corners - 1 ? cidx + 1 : 0;  // wrap around
    auto next_sd_value = sd_values[next_corner];
    auto current_sd_value_sign = math::signum(current_sd_value);
    auto next_sd_value_sign    = math::signum(next_sd_value);

    // std::cout << "(" << cidx << ", " << next_corner << ")"
    //           << ", (" << current_sd_value << ", " << next_sd_value << ")"
    //           << ", (" << current_sd_value_sign << ", " << next_sd_value_sign
    //           << ")"
    //           << "( " << shape_corners[cidx] << ", "
    //           << shape_corners[next_corner] << " )" << std::endl;

    // if a corner is crossed by the zero level set it is part of both polygons
    // (inside and outside), otherwise it is only part of one polygon
    if (current_sd_value_sign == 0) {  // TODO: use approx + suitable epsilon
      // std::cout << "current_sd_value_sign == 0" << std::endl;
      std::get<0>(shapes).push_back(shape_corners[cidx]);
      std::get<1>(shapes).push_back(shape_corners[cidx]);
      sg_values_p0.push_back(0);
      sg_values_p1.push_back(0);
      ++no_cutpoints_found;
      continue;  // we are then done since we found the cut points
    } else if (current_sd_value_sign == 1) {
      // std::cout << "current_sd_value_sign == 1" << std::endl;
      std::get<0>(shapes).push_back(shape_corners[cidx]);
      sg_values_p0.push_back(1);
    } else if (current_sd_value_sign == -1) {
      // std::cout << "current_sd_value_sign == -1" << std::endl;
      std::get<1>(shapes).push_back(shape_corners[cidx]);
      sg_values_p1.push_back(-1);
    } else {
      __builtin_unreachable();
      HM3_FATAL_ERROR("cannot happen");
    }

    // If the sign of the next corner is the same, this edge is not intersected,
    // so we just move on to the next edge
    // If the sign of the next corner is zero, it will be handled in the next
    // iteration
    if (current_sd_value_sign == next_sd_value_sign
        || next_sd_value_sign == 0) {
      // std::cout << "no cut point found" << std::endl;
      continue;
    }

    // Otherwise this edge is intersected by the zero level-set, so find the
    // point in the edge with value 0:
    point<2> x_cp = point_with_value(
     line<Nd>::through(shape_corners[cidx], shape_corners[next_corner]),
     {{current_sd_value, next_sd_value}}, 0.);
    // std::cout << "cut point " << no_cutpoints_found << " found:" << x_cp
    //           << std::endl;
    ++no_cutpoints_found;

    // and we add it to both polygons
    std::get<0>(shapes).push_back(x_cp);
    std::get<1>(shapes).push_back(x_cp);
    sg_values_p0.push_back(0);
    sg_values_p1.push_back(0);

    // and we move to the next edge.
  }
  // std::cout << "ncpf: " << no_cutpoints_found << std::endl;

  // remove polygons containing only points where the level-set is zero:
  if (all_of(sg_values_p0, [](auto i) { return i == 0; })) {
    std::get<0>(shapes).clear();
  }
  if (all_of(sg_values_p1, [](auto i) { return i == 0; })) {
    std::get<1>(shapes).clear();
  }

  // if (no_cutpoints_found == 1) {
  //   if (std::get<0>(shapes).size() == 1) { std::get<0>(shapes).clear(); }
  //   if (std::get<1>(shapes).size() == 1) { std::get<1>(shapes).clear(); }
  // }

  HM3_ASSERT(
   no_cutpoints_found == 1 || no_cutpoints_found == 2,
   "this cannot handle shapes intersected by multiple planes of the same"
   "signed-distance field");

  // Check that the result are closed polygons
  HM3_ASSERT(std::get<0>(shapes).size() > 2 || std::get<0>(shapes).size() == 0,
             "shape0 size: {}", std::get<0>(shapes).size());
  HM3_ASSERT(std::get<1>(shapes).size() > 2 || std::get<1>(shapes).size() == 0,
             "shape1 size: {}", std::get<1>(shapes).size());

  return shapes;
}

}  // namespace geometry
}  // namespace hm3
