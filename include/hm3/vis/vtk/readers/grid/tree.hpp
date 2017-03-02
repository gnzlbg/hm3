#pragma once
/// \file
///
/// VTK Reader for hm3::tree::tree<Nd>
#ifdef HM3_ENABLE_VTK
#include <hm3/vis/vtk/readers/grid/reader.hpp>

namespace hm3 {
namespace vis {
namespace vtk {
namespace grid {

/// Tree grid reader
template <dim_t Nd>
struct tree : reader<Nd> {
  using typename reader<Nd>::grid_t;
  using child_pos = ::hm3::tree::child_pos<grid_t::dimension()>;
  using cpidx_t   = ::hm3::tree::cpidx_t;

  using reader<Nd>::cell_data;
  using reader<Nd>::grid;

  tree() = default;

 protected:
  /// Register tree fields
  virtual void register_cell_data() noexcept override {
    /// Node index:
    cell_data.push("idx", [](auto i) { return *i; });
    /// Node level:
    cell_data.push("level", [&](auto&& i) { return *grid->level(i); });
    /// Parent nodes:
    cell_data.push("parent", [&](auto&& i) { return *grid->parent(i); });
    // Node children :
    auto compute_children = [&](auto&& i, cpidx_t p) {
      const auto c = grid->child(i, child_pos{p});
      return c ? *c : -1;
    };
    auto children_names = [](sint_t c) { return std::to_string(c); };
    cell_data.push("children", std::move(compute_children),
                   grid_t::no_children(), std::move(children_names));
    /// TODO: node neighbors
    // if (Nd > 0) {
    //   auto ns = manifold_neighbors<Nd, 1>{};
    //   cell_data.load("face_neighbors",
    //                  [&](auto&& n, auto&& c) {
    //                    auto nghbr = node_neighbor(t, n, ns.idx(c));
    //                    return nghbr ? static_cast<int_t>(*nghbr) : int_t{-1};
    //                  },
    //                  ns.size());
    // }
    // if (Nd > 1) {
    //   auto ns = manifold_neighbors<Nd, 2>{};
    //   cell_data.load("edge_neighbors",
    //                  [&](auto&& n, auto&& c) {
    //                    auto nghbr = node_neighbor(t, n, ns.idx(c));
    //                    return nghbr ? static_cast<int_t>(*nghbr) : int_t{-1};
    //                  },
    //                  ns.size());
    // }
    // if (Nd > 2) {
    //   auto ns = manifold_neighbors<Nd, 3>{};
    //   cell_data.load("corner_neighbors",
    //                  [&](auto&& n, auto&& c) {
    //                    auto nghbr = node_neighbor(t, n, ns.idx(c));
    //                    return nghbr ? static_cast<int_t>(*nghbr) : int_t{-1};
    //                  },
    //                  ns.size());
    // }

    reader<Nd>::register_cell_data();
  }
};

/// Makes a tree grid reader of dimension `nd`
template <dim_t Nd>
auto make_tree() -> std::unique_ptr<::hm3::vis::vtk::reader> {
  return std::make_unique<tree<Nd>>();
}

/// Makes a tree grid reader from io::session block of type
/// tree.
inline auto make_tree(io::json const& b)
 -> std::unique_ptr<::hm3::vis::vtk::reader> {
  string block_type = io::read_field(b, "type", HM3_AT_);
  if (block_type != tree_type()) {
    HM3_FATAL_ERROR("Cannot make a tree reader from a block of type \"{}\"",
                    block_type);
  }

  if (b["files"].empty()) {
    HM3_FATAL_ERROR("tree contains no files:\n\n{}\n\n", b);
  }

  dim_t nd = io::read_file_field(b, "spatial_dimension", HM3_AT_);
  switch (nd) {
    case 1: {
      return make_tree<1>();
    }
    case 2: {
      return make_tree<2>();
    }
    case 3: {
      return make_tree<3>();
    }
    default: {
      HM3_FATAL_ERROR("unsupported #of spatial dimensions, nd = {}", nd);
    }
  }
}

}  // namespace grid
}  // namespace vtk
}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
