#pragma once
/// \file
///
/// Line segment
#include <hm3/geometry/point.hpp>

namespace hm3 {
namespace geometry {

/// Line segment: line bounded by two end-points
template <dim_t Nd>  //
struct segment : dimensional<Nd> {
  using point_t  = point<Nd>;
  using vector_t = vector<Nd>;
  point_t x_0, x_1;
  /// Line segment that goes through the 2 points \p b and \p e : [b, e]
  static segment through(point_t b, point_t e) noexcept {
    HM3_ASSERT(b != e, "line through two equal points: {}!", b);
    return segment{b, e};
  }

  /// Line segment at \p p in direction \p dir : [p, p+dir]
  static segment at(point_t p, vector_t dir) noexcept {
    HM3_ASSERT(dir != vector_t::constant(0.), "line with zero direction: {}!",
               dir);
    return segment{p, point_t{p() + dir()}};
  }

  auto operator()() const noexcept {
    return Eigen::ParametrizedLine<num_t, Nd>::Through(x_0(), x_1());
  }

  template <typename Rng, CONCEPT_REQUIRES_(Range<Rng>{})>
  constexpr segment(Rng&& rng) noexcept {
    HM3_ASSERT(ranges::distance(rng) == 2, "?");
    auto b = begin(rng);
    x_0    = *b;
    ++b;
    x_1 = *b;
    HM3_ASSERT(x_0 != x_1, "line through two equal points: {}!", x_0);
  }

  constexpr segment(point_t from, point_t to) : x_0(from), x_1(to) {}
  constexpr segment()               = default;
  constexpr segment(segment const&) = default;
  constexpr segment(segment&&)      = default;
  constexpr segment& operator=(segment const&) = default;
  constexpr segment& operator=(segment&&) = default;
};

template <dim_t Nd>
bool operator==(segment<Nd> const& l, segment<Nd> const& r) noexcept {
  return l.x_0 == r.x_0 and l.x_1 == r.x_1;
}

template <dim_t Nd>
bool operator!=(segment<Nd> const& l, segment<Nd> const& r) noexcept {
  return !(l == r);
}

}  // namespace geometry
}  // namespace hm3
