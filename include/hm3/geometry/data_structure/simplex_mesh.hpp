#pragma once
/// \file
///
/// Mesh of simplices
#include <hm3/geometry/bvh.hpp>
#include <hm3/geometry/simplex/simplices.hpp>

template <dim_t Nd>  //
struct mesh : simplices<Nd>, bvh<Nd> {
  using simplices_t = simplices<Nd>;
  using bvh_t       = bvh<Nd>;

  template <typename SimplexRange>
  mesh(SimplexRange const& simplices) : simplices_t(simplices) {
    this->rebuild(static_cast<simplices_t const&>(*this));
  }
};

template <dim_t Nd>
bool intersects(aabb<Nd> const& a, mesh<Nd> const& m) noexcept {}
