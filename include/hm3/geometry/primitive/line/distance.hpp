#pragma once
/// \file
///
/// Minimum distance between two lines and point and line.
#include <hm3/geometry/algorithm/distance.hpp>
#include <hm3/geometry/primitive/line/coincidental.hpp>
#include <hm3/geometry/primitive/line/direction.hpp>
#include <hm3/geometry/primitive/line/line.hpp>
#include <hm3/geometry/primitive/line/parallel.hpp>

namespace hm3::geometry::line_primitive {

/// Minimum distance between the line \p l and the point \p p.
template <dim_t Nd>
num_t minimum_distance(line<Nd> const& l, point<Nd> const& p) {
  return l().distance(p());
}

/// Minimum distance between the lines \p a and \p b.
num_t minimum_distance(line<1> const&, line<1> const&) { return 0.; }

/// Minimum distance between the lines \p a and \p b.
num_t minimum_distance(line<2> const& a, line<2> const& b) {
  // TODO: optimize some common computation of parallel and coincidental
  if (parallel(direction(a), direction(b))) {
    if (coincidental(a, b)) { return 0.; }  // equal => zero distance
    // parallel but not coincidental:
    auto b_o = b.origin();
    auto p   = a().projection(b_o());
    return (p - b_o()).norm();
  }
  // not parallel: intersect in one point => zero distance
  return 0.;
}

num_t minimum_distance(line<3> const& a, line<3> const& b) {
  auto da = direction(a);
  auto db = direction(b);
  if (parallel(da, db)) {
    if (coincidental(a, b)) { return 0.; }  // equal => zero distance
    // parallel but not coincidental:
    auto b_o = b.origin();
    auto p   = a().projection(b_o());
    return (p - b_o()).norm();
  }
  // might intersect or be skewed

  auto da_cross_db = da().cross(db());
  auto oa          = a.origin();
  auto ob          = b.origin();
  return std::abs((da_cross_db / da_cross_db.norm()).dot(oa() - ob()));
}

}  // namespace hm3::geometry::line
