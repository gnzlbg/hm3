#pragma once
/// \file
///
/// Line segment.
///
/// TODO: make segment and line interfaces consistent with respect to through
/// and at (these are not needed anymore since the constructor can take p_t, p_t
/// and p_t, v_t).
#include <hm3/geometry/primitive/point.hpp>

namespace hm3::geometry {

/// Segment primitive.
namespace segment_primitive {

/// Line segment: line bounded by two end-points.
///
/// \tparam Nd Number of spatial dimensions.
template <dim_t Nd>
struct segment : ranked<Nd, 1> {
  using point_t           = point<Nd>;
  using vector_t          = vec<Nd>;
  using vertex_index_type = dim_t;
  using face_index_type   = dim_t;

  array<point_t, 2> xs_;

  /// Access the endpoint \p i of the segment.
  ///
  /// \pre \p i must be in range [0, 2).
  point_t& x(dim_t i) noexcept {
    HM3_ASSERT(i < 2, "index {} out-of-bounds [0, 2)", i);
    return xs_[i];
  }

  /// Access the endpoint \p i of the segment.
  ///
  /// \pre \p i must be in range [0, 2).
  point_t const& x(dim_t i) const noexcept {
    HM3_ASSERT(i < 2, "index {} out-of-bounds [0, 2)", i);
    return xs_[i];
  }

  /// Line segment that goes through the 2 points \p b and \p e : [b, e]
  static segment through(point_t b, point_t e) noexcept {
    HM3_ASSERT(b != e, "line through two equal points: {} == {}!", b, e);
    return segment{b, e};
  }

  /// Line segment at \p p in direction \p dir : [p, p+dir]
  static segment at(point_t p, vector_t dir) noexcept {
    HM3_ASSERT(dir != vector_t::constant(0.), "line with zero direction: {}!",
               dir);
    return segment{p, point_t{p() + dir()}};
  }

  /// Obtains an unbounded line from the segment.
  auto line() const noexcept { return geometry::line<Nd>::through(x(0), x(1)); }

  /// Obtains an unbounded Eigen::ParametrizedLine from the segment.
  auto operator()() const noexcept {
    return Eigen::ParametrizedLine<num_t, Nd>::Through(x(0)(), x(1)());
  }

  /// Construct a segment from the range of points \p rng.
  ///
  /// \pre `size(rng) == 2`.
  template <typename Rng,
            CONCEPT_REQUIRES_(
             Range<Rng>{}
             and ranges::Assignable<point<Nd>, ranges::range_value_t<Rng>>{})>
  constexpr segment(Rng&& rng) noexcept {
    HM3_ASSERT(ranges::distance(rng) == 2, "?");
    ranges::copy(rng, begin(xs_));
    HM3_ASSERT(x(0) != x(1), "line through two equal points: {}!", x(0));
  }

  constexpr segment(point_t from, point_t to) : xs_{{from, to}} {
    HM3_ASSERT(x(0) != x(1), "segment through two equal points: {} == {}!",
               x(0), x(1));
  }
  constexpr segment()               = default;
  constexpr segment(segment const&) = default;
  constexpr segment(segment&&)      = default;
  constexpr segment& operator=(segment const&) = default;
  constexpr segment& operator=(segment&&) = default;
};

template <dim_t Nd>
bool operator==(segment<Nd> const& l, segment<Nd> const& r) noexcept {
  return l.x(0) == r.x(0) and l.x(1) == r.x(1);
}

template <dim_t Nd>
bool operator!=(segment<Nd> const& l, segment<Nd> const& r) noexcept {
  return !(l == r);
}

}  // namespace segment_primitive

using segment_primitive::segment;

}  // namespace hm3::geometry
