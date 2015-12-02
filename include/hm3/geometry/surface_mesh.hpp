#pragma once
/// \file surface_mesh.hpp
#include <hm3/geometry/simplex.hpp>
#include <hm3/solver/utility/grid.hpp>

namespace hm3 {
namespace geometry {

/// Uses the tree, one surface per tree node
///
/// TODO: this is as simple as it gets for a parallel distributed surface mesh,
/// lots of optimizations possible...
template <uint_t Nd, typename Data>  //
struct surface_mesh {
  struct surface_t : simplex<Nd>, Data {};

  solver::state::grid<Nd> tree;
  std::vector<surface_t> surfaces;

  surface_mesh(tree_t& t, grid_idx g, uint_t surface_capacity,
               uint_t point_capacity)
   : tree(t, g, grid_node_capacity(surface_capacity))
   , points(point_capacity)
   , surfaces(surface_capacity)
   , surface_data(surface_capacity) {}

  // push
  // pop
  // displace_all_surfaces_by_vec
  auto operator()() { return }
};

}  // namespace geometry

}  // namespace hm3
