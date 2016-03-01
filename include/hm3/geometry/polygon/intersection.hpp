#pragma once
/// \file
///
/// Intersection of polygons
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/polygon/concept.hpp>
#include <hm3/geometry/sd/concept.hpp>
#include <hm3/interpolation/linear.hpp>
#include <hm3/utility/inline_vector.hpp>

namespace hm3 {
namespace geometry {

template <typename Shape, typename SDFunction,
          CONCEPT_REQUIRES_(Polygon<Shape>{})>
bool is_intersected(Shape const& s, SDFunction&& sd) noexcept {
  const auto shape_corners = corners(s);
  const suint_t no_corners = size(shape_corners);

  auto sg = math::signum(sd(shape_corners[0]));
  for (suint_t c = 1; c != no_corners; ++c) {
    auto sg_c = math::signum(sd(shape_corners[c]));
    if (sg == 0) { sg = sg_c; }
    if (sg != sg_c) { return true; }
  }
  return false;
}

template <typename Shape, typename SDFunction>
bool is_completely_inside(Shape&& s, SDFunction&& sd) noexcept {
  return sd(centroid(s)) > 0. and !is_intersected(s, sd);
}

template <typename Shape>  //
struct intersection {
  Shape inside, outside, surface;

  inline_vector<sint_t, Shape::capacity()> signum_inside, signum_outside;
};

template <typename Shape, typename SDFunction, uint_t Nd = Shape::dimension(),
          CONCEPT_REQUIRES_(Polygon<Shape>{}  // and ResizablePolygon<Shape>
                            and SignedDistance<SDFunction, Nd>{}),
          uint_t MaxNp = Shape::max_points()>
intersection<Shape> intersect(Shape const& s, SDFunction&& sd) noexcept {
  intersection<Shape> cut;

  const auto shape_corners = corners(s);
  const suint_t no_corners = size(shape_corners);
  auto corner_ids          = view::iota(suint_t{0}, no_corners);

  // Compute signed distance values at the corners once:
  inline_vector<num_t, MaxNp> sd_at_corners;
  for (auto c : shape_corners) { sd_at_corners.push_back(sd(c)); }

  // Index of the next corner, wraps around for the last corner:
  auto next_corner
   = [&](auto&& cidx) { return (cidx != no_corners - 1) ? cidx + 1 : 0; };

  suint_t no_cutpoints_found = 0;
  /// Adds corners and cut points to each polygon
  for (auto cidx : corner_ids) {
    const auto corner_sd           = sd_at_corners[cidx];
    const auto n_cidx              = next_corner(cidx);
    const auto next_corner_sd      = sd_at_corners[n_cidx];
    const auto corner_sd_sign      = math::signum(corner_sd);
    const auto next_corner_sd_sign = math::signum(next_corner_sd);

    // if a corner is crossed by the zero level set it is part of both polygons
    // (inside and outside) and the cut surface, otherwise it is only part of
    // one polygon
    if (corner_sd_sign == 0) {
      cut.inside.push_back(shape_corners[cidx]);
      cut.outside.push_back(shape_corners[cidx]);
      cut.signum_inside.push_back(corner_sd_sign);
      cut.signum_outside.push_back(corner_sd_sign);
      cut.surface.push_back(shape_corners[cidx]);
      ++no_cutpoints_found;
      continue;  // we are then done since we found the cut points
    } else if (corner_sd_sign == 1) {
      cut.inside.push_back(shape_corners[cidx]);
      cut.signum_inside.push_back(corner_sd_sign);
    } else if (corner_sd_sign == -1) {
      cut.outside.push_back(shape_corners[cidx]);
      cut.signum_outside.push_back(corner_sd_sign);
    } else {
      HM3_FATAL_ERROR("unreachable");
    }

    // If:
    // - the sign of the next corner is the same, this edge is not intersected,
    //   so we just move on to the next edge.
    // - the sign of the next corner is zero, it will be handled in the next
    //   iteration.
    if (corner_sd_sign == next_corner_sd_sign || next_corner_sd_sign == 0) {
      continue;
    }

    // Otherwise this edge is intersected by the zero level-set, so find the
    // point in the edge with value 0 by interpolating linearly:
    auto x_cp = ip::linear::point_with_value(
     0., shape_corners[cidx], shape_corners[n_cidx], corner_sd, next_corner_sd);
    ++no_cutpoints_found;

    // Cut points are part fo both polygons and part of the surface:
    cut.inside.push_back(x_cp);
    cut.outside.push_back(x_cp);
    cut.signum_inside.push_back(0);
    cut.signum_outside.push_back(0);
    cut.surface.push_back(x_cp);

    // and we move to the next edge.
  }

  // remove polygons containing only points where the level-set is zero:
  if (all_of(cut.signum_inside, [](auto i) { return i == 0; })) {
    cut.inside.clear();
  }
  if (all_of(cut.signum_outside, [](auto i) { return i == 0; })) {
    cut.outside.clear();
  }

  HM3_ASSERT(
   no_cutpoints_found == 1 || no_cutpoints_found == 2,
   "this cannot handle shapes intersected by multiple planes of the same"
   "signed-distance field");

  // Check that the result are closed polygons (not a single point or a line)
  HM3_ASSERT(cut.inside.size() > 2 || cut.inside.size() == 0, "shape0 size: {}",
             cut.inside.size());
  HM3_ASSERT(cut.outside.size() > 2 || cut.outside.size() == 0,
             "shape1 size: {}", cut.outside.size());

  HM3_ASSERT(size(cut.surface) == no_cutpoints_found,
             "{} cut points found but the cut.surface has {} points",
             no_cutpoints_found, size(cut.surface));

  return cut;
}

}  // namespace geometry
}  // namespace hm3
