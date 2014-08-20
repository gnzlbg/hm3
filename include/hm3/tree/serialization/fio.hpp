#pragma once
/// \file fio.hpp Serialization to HM3's File I/O
#include <hm3/tree/tree.hpp>
#include <hm3/tree/algorithm/dfs_sort.hpp>
#include <hm3/io/file.hpp>
#include <hm3/io/session.hpp>
#include <hm3/io/client.hpp>

namespace hm3 {
namespace tree {

/// Maps arrays in the file descriptor to memory addresses
template <uint_t nd> void map_arrays(io::file& f, tree<nd> const& t) {
  f.field("parents", reinterpret_cast<uint_t const*>(t.parents_.get()),
          *t.no_sibling_groups(t.size()))
   .field("first_children",
          reinterpret_cast<uint_t const*>(t.first_children_.get()), *t.size());
  // f.field("sibling_to_parent_edges",
  //         reinterpret_cast<uint_t const*>(t.parents_.get()),
  //         *t.no_sibling_groups(t.size()))
  //  .field("parent_to_children_edges",
  //         reinterpret_cast<uint_t const*>(t.first_children_.get()),
  //         *t.size());
}

/// Returns a yet to be read tree from a file descriptor \p f
template <uint_t nd>
tree<nd> from_file_unread(tree<nd> const&, io::file& f,
                          node_idx node_capacity) {
  // Check the tree dimension:
  auto tree_dim = f.constant("spatial_dimension", int64_t{});
  if (nd != tree_dim) {
    HM3_TERMINATE("Cannot read tree: tree dimension mismatch (file dim: {}, "
                  "memory dim: {})!",
                  tree_dim, nd);
  }
  // Set and check the node capacity:
  auto no_nodes = f.constant("no_tree_nodes", int64_t{});
  if (!node_capacity) {
    node_capacity = node_idx{no_nodes};
  } else if (*node_capacity < no_nodes) {
    HM3_TERMINATE(
     "Cannot read tree: insuficient capacity (no_nodes: {}, capacity: {})!",
     no_nodes, node_capacity);
  }

  // Construct a tree with the given capacity and number of nodes:
  tree<nd> t(*node_capacity);
  t.size_                     = node_idx{no_nodes};
  t.first_free_sibling_group_ = t.sibling_group(t.size());

  // Map tree arrays to the file:
  map_arrays(f, t);

  // Move the tree out of the function:
  static_assert(std::is_move_constructible<tree<nd>>{},
                "if the tree is not move constructible mapping the arrays "
                "fails (they will be mapped to the wrong addresses in memory)");
  return t;
}

/// Reads tree from file descriptor \p f
template <uint_t nd>
tree<nd> from_file(tree<nd> const&, io::file& f,
                   node_idx node_capacity = node_idx{}) {
  auto&& t = from_file_unread(tree<nd>{}, f, node_capacity);
  f.read_arrays();
  return t;
}

/// Appends constants and map arrays to file \p f
template <uint_t nd> void to_file_unwritten(io::file& f, tree<nd> const& t) {
  f.field("spatial_dimension", nd).field("no_tree_nodes", *t.size());
  map_arrays(f, t);
}

}  // namespace tree
}  // namespace hm3
