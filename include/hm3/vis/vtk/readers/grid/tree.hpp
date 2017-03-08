#pragma once
/// \file
///
/// VTK Reader for hm3::tree::tree<Ad>
#ifdef HM3_ENABLE_VTK
#include <hm3/vis/vtk/readers/grid/reader.hpp>

namespace hm3::vis::vtk::grid {

/// Tree grid reader
template <dim_t Ad>
struct tree : reader<Ad> {
  using typename reader<Ad>::grid_t;
  using child_pos = ::hm3::tree::child_pos<ad_v<grid_t>>;
  using cpidx_t   = ::hm3::tree::cpidx_t;

  using reader<Ad>::cell_data;
  using reader<Ad>::grid;

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
    // if constexpr(Ad > 0) {
    //   auto ns = manifold_neighbors<Ad, 1>{};
    //   cell_data.load("face_neighbors",
    //                  [&](auto&& n, auto&& c) {
    //                    auto nghbr = node_neighbor(t, n, ns.idx(c));
    //                    return nghbr ? static_cast<int_t>(*nghbr) : int_t{-1};
    //                  },
    //                  ns.size());
    // }
    // if constexpr(Ad > 1) {
    //   auto ns = manifold_neighbors<Ad, 2>{};
    //   cell_data.load("edge_neighbors",
    //                  [&](auto&& n, auto&& c) {
    //                    auto nghbr = node_neighbor(t, n, ns.idx(c));
    //                    return nghbr ? static_cast<int_t>(*nghbr) : int_t{-1};
    //                  },
    //                  ns.size());
    // }
    // if constexpr(Ad > 2) {
    //   auto ns = manifold_neighbors<Ad, 3>{};
    //   cell_data.load("corner_neighbors",
    //                  [&](auto&& n, auto&& c) {
    //                    auto nghbr = node_neighbor(t, n, ns.idx(c));
    //                    return nghbr ? static_cast<int_t>(*nghbr) : int_t{-1};
    //                  },
    //                  ns.size());
    // }

    reader<Ad>::register_cell_data();
  }
};

/// Makes a tree grid reader of dimension `nd`
template <dim_t Ad>
auto make_tree() -> std::unique_ptr<::hm3::vis::vtk::reader> {
  return std::make_unique<tree<Ad>>();
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

  dim_t ad = io::read_file_field(b, "spatial_dimension", HM3_AT_);
  switch (ad) {
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
      HM3_FATAL_ERROR("unsupported #of spatial dimensions, ad = {}", ad);
    }
  }
}

}  // namespace hm3::vis::vtk::grid
#endif  // HM3_ENABLE_VTK
