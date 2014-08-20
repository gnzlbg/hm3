#pragma once
/// \file single_fio.hpp Serialization of single hierarchical Cartesian grid to
/// HM3's File I/O
#include <hm3/tree/serialization/fio.hpp>
#include <hm3/grid/hc/single.hpp>

namespace hm3 {
namespace grid {
namespace hc {

/// Returns a yet to be read grid from a file descriptor \p f
template <uint_t nd>
single<nd> from_file_unread(single<nd> const&, io::file& f,
                            tree_node_idx node_capacity) {
  auto root_node
   = geometry::square<nd>{f.constant("root_node_center", geometry::point<nd>{}),
                          f.constant("root_node_length", num_t{})};

  single<nd> g(root_node,
               from_file_unread<nd>(tree::tree<nd>{}, f, node_capacity));
  // Move the grid hierarchical Cartesian grid out of the function:
  static_assert(std::is_move_constructible<single<nd>>{},
                "if the grid is not move constructible mapping the arrays "
                "fails (they will be mapped to the wrong addresses in memory)");
  return g;
}

/// Reads grid from file descriptor \p f
template <uint_t nd>
single<nd> from_file(single<nd> const&, io::file& f,
                     tree_node_idx node_capacity = tree_node_idx{}) {
  auto&& g = from_file_unread<nd>(single<nd>{}, f, node_capacity);
  f.read_arrays();
  return g;
}

/// Appends constants and map arrays to file \p f
template <uint_t nd> void to_file_unwritten(io::file& f, single<nd> const& g) {
  to_file_unwritten(f, static_cast<tree::tree<nd> const&>(g));
  f.field("root_node_center", geometry::point<nd>{center(g.bounding_box())})
   .field("root_node_length", geometry::length(g.bounding_box()));
}

}  // namespace hc
}  // namespace grid
}  // namespace hm3
