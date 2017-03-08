#pragma once
/// \file
///
/// Serialization of single hierarchical Cartesian grid to HM3's File I/O
#include <hm3/grid/hierarchical/cartesian/single.hpp>
#include <hm3/grid/hierarchical/tree/serialization/fio.hpp>

namespace hm3::grid::hierarchical::cartesian {

/// Returns a yet to be read grid from a file descriptor \p f
template <dim_t Ad>
single<Ad> from_file_unread(single<Ad> const&, io::file& f,
                            tree_node_idx node_capacity) {
  auto root_node
   = geometry::box<Ad>{f.constant("root_node_center", geometry::point<Ad>{}),
                       f.constant("root_node_length", num_t{})};

  single<Ad> g(root_node,
               from_file_unread<Ad>(tree::tree<Ad>{}, f, node_capacity));
  // Move the grid hierarchical Cartesian grid out of the function:
  static_assert(std::is_move_constructible<single<Ad>>{},
                "if the grid is not move constructible mapping the arrays "
                "fails (they will be mapped to the wrong addresses in memory)");
  return g;
}

/// Reads grid from file descriptor \p f
template <dim_t Ad>
single<Ad> from_file(single<Ad> const&, io::file& f,
                     tree_node_idx node_capacity = tree_node_idx{}) {
  auto&& g = from_file_unread<Ad>(single<Ad>{}, f, node_capacity);
  f.read_arrays();
  return g;
}

/// Appends constants and map arrays to file \p f
template <dim_t Ad>
void to_file_unwritten(io::file& f, single<Ad> const& g) {
  to_file_unwritten(f, static_cast<tree::tree<Ad> const&>(g));
  f.field("root_node_center",
          geometry::point<Ad>{geometry::centroid(g.bounding_box())})
   .field("root_node_length", geometry::bounding_length(g.bounding_box()));
}

}  // namespace hm3::grid::hierarchical::cartesian
