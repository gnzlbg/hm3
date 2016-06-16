#pragma once
/// \file
///
/// Triangle
#include <hm3/geometry/point.hpp>

namespace hm3 {
namespace geometry {

template <dim_t Nd>  //
struct triangle : dimensional<Nd> {
  using point_t = geometry::point<Nd>;
  array<point_t, 3> vertices;

  auto operator()() const noexcept {
    return Eigen::Hyperplane<num_t, Nd>::Through(vertices[0](), vertices[1](),
                                                 vertices[2]());
  }
  template <typename Rng, CONCEPT_REQUIRES_(Range<Rng>{})>
  constexpr triangle(Rng&& rng) noexcept {
    HM3_ASSERT(ranges::distance(rng) == 3, "?");
    ranges::copy(rng, begin(vertices));
    if (Nd == 2) {
      /// HM3_ASSERT() ccw order
    }
  }

  constexpr triangle()                = default;
  constexpr triangle(triangle const&) = default;
  constexpr triangle(triangle&&)      = default;
  constexpr triangle& operator=(triangle const&) = default;
  constexpr triangle& operator=(triangle&&) = default;
};

template <dim_t Nd>  //
constexpr bool empty(triangle<Nd> const&) noexcept {
  return false;
}

template <dim_t Nd>
bool operator==(triangle<Nd> const& l, triangle<Nd> const& r) noexcept {
  return equal(l.vertices, r.vertices);
}

template <dim_t Nd>
bool operator!=(triangle<Nd> const& l, triangle<Nd> const& r) noexcept {
  return !(l == r);
}

}  // namespace geometry
}  // namespace hm3
