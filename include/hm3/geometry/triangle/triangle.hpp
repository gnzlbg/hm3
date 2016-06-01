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
    copy(rng, begin(vertices));
  }
};

}  // namespace geometry
}  // namespace hm3
