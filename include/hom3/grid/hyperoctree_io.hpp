#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file \brief Implements an n-dimensional Hyperoctree
////////////////////////////////////////////////////////////////////////////////
/// Include:
#include <fstream>
#include <hom3/grid/hyperoctree.hpp>
////////////////////////////////////////////////////////////////////////////////
/// File macros:
#define ENABLE_DBG_ 0  // needs undef
#include <hom3/core/dbg.hpp>
////////////////////////////////////////////////////////////////////////////////

namespace hom3 {

/// \brief n-dimensional Hyperoctree implementation
namespace hyperoctree {

namespace format {
struct dot_t {};
static const constexpr dot_t dot{};
}

template <class Hyperoctree, class HyperoctreeNodeRange, class Ostream>
void pretty_print(Hyperoctree const& tree,
                  HyperoctreeNodeRange const& node_range, Ostream& os,
                  format::dot_t) {
  os << "digraph graphname {\n";
  for (auto n : node_range) {
    for (auto c : tree.child_positions()) {
      if (is_valid(tree.child(n, c))) {
        os << n << " -> " << tree.child(n, c) << ";\n";
      }
    }
  }
  os << "}\n";
}

template <class Hyperoctree, class HyperoctreeNodeRange>
void pretty_print(Hyperoctree const& tree,
                  HyperoctreeNodeRange const& node_range, const String fname) {
  std::ofstream file;
  file.open(fname + ".dot");
  pretty_print(tree, node_range, file, format::dot);
}




}  // namespace hyperoctree
}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
/// File macros:
#undef ENABLE_DBG_
////////////////////////////////////////////////////////////////////////////////
