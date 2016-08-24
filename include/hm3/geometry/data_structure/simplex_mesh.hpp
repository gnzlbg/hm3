#pragma once
/// \file
///
/// Mesh of simplices
#include <hm3/geometry/data_structure/bvh.hpp>
#include <hm3/geometry/data_structure/simplex_array.hpp>

template <dim_t Nd>
struct mesh : simplices<Nd>, bvh<Nd> {
  using array_t = simplex_array<Nd>;
  using bvh_t   = bvh<Nd>;

  template <typename SimplexRange>
  mesh(SimplexRange const& simplices) : simplices_t(simplices) {
    this->rebuild(static_cast<simplices_t const&>(*this));
  }
};

template <dim_t Nd>
bool intersects(aabb<Nd> const& a, mesh<Nd> const& m) noexcept {}
