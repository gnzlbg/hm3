#pragma once
/// \file
///
/// Serialization of single hierarchical Cartesian grid to HM3's File I/O
#include <hm3/grid/hierarchical/cartesian/single.hpp>
#include <hm3/grid/hierarchical/tree/serialization/fio.hpp>

namespace hm3 {
namespace grid {
namespace hierarchical {
namespace cartesian {

/// Returns a yet to be read grid from a file descriptor \p f
template <uint_t Nd>
single<Nd> from_file_unread(single<Nd> const&, io::file& f,
                            tree_node_idx node_capacity) {
  auto root_node
   = geometry::square<Nd>{f.constant("root_node_center", geometry::point<Nd>{}),
                          f.constant("root_node_length", num_t{})};

  single<Nd> g(root_node,
               from_file_unread<Nd>(tree::tree<Nd>{}, f, node_capacity));
  // Move the grid hierarchical Cartesian grid out of the function:
  static_assert(std::is_move_constructible<single<Nd>>{},
                "if the grid is not move constructible mapping the arrays "
                "fails (they will be mapped to the wrong addresses in memory)");
  return g;
}

/// Reads grid from file descriptor \p f
template <uint_t Nd>
single<Nd> from_file(single<Nd> const&, io::file& f,
                     tree_node_idx node_capacity = tree_node_idx{}) {
  auto&& g = from_file_unread<Nd>(single<Nd>{}, f, node_capacity);
  f.read_arrays();
  return g;
}

/// Appends constants and map arrays to file \p f
template <uint_t Nd> void to_file_unwritten(io::file& f, single<Nd> const& g) {
  to_file_unwritten(f, static_cast<tree::tree<Nd> const&>(g));
  f.field("root_node_center", geometry::point<Nd>{centroid(g.bounding_box())})
   .field("root_node_length", geometry::length(g.bounding_box()));
}

}  // namespace cartesian
}  // namespace hierarchical
}  // namespace grid
}  // namespace hm3
