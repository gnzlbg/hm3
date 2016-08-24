#pragma once
/// \file
///
/// Bounding length of a polyline.
#include <hm3/geometry/algorithm/bounding_length.hpp>
#include <hm3/geometry/primitive/polyline/bounding_volume.hpp>

namespace hm3::geometry::polyline_primitive {

/// Axis-Aligned Bounding Box of the polyline \p p.
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polyline<UP, Nd>{})>
constexpr num_t bounding_length(P&& p, dim_t d) {
  num_t min_ = 0., max_ = 0.;
  for (auto&& v : vertices(p)) {
    min_ = std::min(min_, v(d));
    max_ = std::max(max_, v(d));
  }
  if (math::approx(min_, max_)) {
    min_ -= math::eps;
    max_ += math::eps;
  }
  return max_ - min_;
}

/// Bounding lengths of the polyline \p p along all spatial components.
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polyline<UP, Nd>{})>
constexpr vec<Nd> all_bounding_lengths(P&& p) noexcept {
  return aabb_primitive::all_bounding_lengths(axis_aligned_bounding_box(p));
}

/// Max bounding length of the polyline \p p.
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polyline<UP, Nd>{})>
constexpr num_t max_bounding_length(P&& p) noexcept {
  auto ls = all_bounding_lengths(p);
  return ls().array().maxCoeff();
}

}  // namespace hm3::geometry::polyline_primitive
