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
  point<Nd> x_0, x_1;
  /// Line that goes through the 2 points \p b and \p e
  static segment<Nd> through(point<Nd> b, point<Nd> e) noexcept {
    return segment<Nd>{b, e};
  }

  /// Line that goes through the point \p p in direction \p dir
  static segment<Nd> at(point<Nd> p, vector<Nd> dir) noexcept {
    return segment<Nd>{b, e};
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
  }

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
