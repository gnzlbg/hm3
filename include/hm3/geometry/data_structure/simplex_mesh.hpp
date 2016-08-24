#pragma once
/// \file
///
/// Mesh of simplices
#include <hm3/geometry/data_structure/bvh.hpp>
#include <hm3/geometry/data_structure/simplex_array.hpp>

namespace hm3::geometry {

template <dim_t Nd>
struct mesh : simplex_array<Nd>, bounding_volume_hierarchy<Nd> {
  using array_t = simplex_array<Nd>;
  using bvh_t   = bounding_volume_hierarchy<Nd>;

  template <typename SimplexRange>
  mesh(SimplexRange const& simplices) : array_t(simplices) {
    this->rebuild(static_cast<array_t const&>(*this));
  }
};

// template <dim_t Nd>
// bool intersects(aabb<Nd> const&, mesh<Nd> const&) noexcept {}

}  // namespace hm3::geometry
