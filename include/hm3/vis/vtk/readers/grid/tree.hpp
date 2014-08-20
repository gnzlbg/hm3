#pragma once
/// \file tree_reader.hpp
#ifdef HM3_ENABLE_VTK
#include <hm3/vis/vtk/readers/grid/reader.hpp>

namespace hm3 {
namespace vis {
namespace vtk {
namespace grid {

/// Tree grid reader
template <uint_t nd> struct tree : reader<nd> {
  using typename reader<nd>::grid_t;
  using child_pos = ::hm3::tree::child_pos<grid_t::dimension()>;

  using reader<nd>::cell_data;
  using reader<nd>::grid;

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
    /// Node children:
    auto compute_children = [&](auto&& i, suint_t p) {
      const auto c = grid->child(i, child_pos{p});
      return c ? *c : -1;
    };
    auto children_names = [](sint_t c) { return std::to_string(c); };
    cell_data.push("children", std::move(compute_children),
                   grid_t::no_children(), std::move(children_names));

    // /// Node neighbors:
    // auto register_neighbor_field = [&](auto manifold) {
    //   /// How to compute the neighbors:
    //   auto compute_neighbors = [&, manifold](auto a) {
    //     return [&, a, s](auto&& i, sint_t c) {
    //       auto neighbor = grid->neighbor(i, manifold[c]);
    //       return is_valid(neighbor) ? neighbor() : -1;
    //     };
    //   };
    //   /// #of neighbors:
    //   auto no_neighbors
    //    = [](auto a, auto s) { return grid_t::no_neighbors(a, s); };

    //   /// Name of neighbor field:
    //   auto neighbor_field = [](auto a, auto s) {
    //     using a_t         = decltype(a);
    //     using s_t         = decltype(s);
    //     string name = "tree_node_neighbors_across_";
    //     if (std::is_same<a_t, across_faces_t<grid_t>>{}) { name += "faces"; }
    //     if (std::is_same<a_t, across_edges_t<grid_t>>{}) { name += "edges"; }
    //     if (std::is_same<a_t, across_corners_t<grid_t>>{}) {
    //       name += "corners";
    //     }
    //     if (std::is_same<a_t, across_all_t<grid_t>>{}) { name += "all"; }
    //     name += "_set_";
    //     if (std::is_same<s_t, at_same_level_t<grid_t>>{}) {
    //       name += "at_same_level";
    //     }
    //     if (std::is_same<s_t, at_children_level_t<grid_t>>{}) {
    //       name += "at_children_level";
    //     }
    //     if (std::is_same<s_t, at_parent_level_t<grid_t>>{}) {
    //       name += "at_parent_level";
    //     }
    //     return name;
    //   };
    //   /// Name of neighbor components:
    //   auto neighbor_names = [&](auto a) {
    //     using a_t = decltype(a);
    //     return [](sint_t c) {
    //       if (std::is_same<a_t, across_faces_t<grid_t>>{}) {
    //         return "face_neighbor_" + std::to_string(c);
    //       }
    //       if (std::is_same<a_t, across_edges_t<grid_t>>{}) {
    //         return "edge_neighbor_" + std::to_string(c);
    //       }
    //       if (std::is_same<a_t, across_corners_t<grid_t>>{}) {
    //         return "corner_neighbor_" + std::to_string(c);
    //       }
    //       if (std::is_same<a_t, across_all_t<grid_t>>{}) {
    //         return "neighbor_" + std::to_string(c);
    //       }
    //     };
    //   };
    //   /// Register the neighbor field:
    //   cell_data.push(neighbor_field(across, set),
    //                  compute_neighbors(across, set), no_neighbors(across,
    //                  set),
    //                  neighbor_names(across));
    // };

    // auto register_neighbor_set = [&](auto set) {
    //   register_neighbor_field(across_faces<grid_t>, set);
    //   if (nd > 1) { register_neighbor_field(across_edges<grid_t>, set); }
    //   if (nd > 2) { register_neighbor_field(across_corners<grid_t>, set); }
    //   if (nd > 1) { register_neighbor_field(across_all<grid_t>, set); }
    // };

    // register_neighbor_set(at_same_level<grid_t>);
    // register_neighbor_set(at_children_level<grid_t>);
    // register_neighbor_set(at_parent_level<grid_t>);

    reader<nd>::register_cell_data();
  }
};

/// Makes a tree grid reader of dimension `nd`
template <uint_t nd>
auto make_tree() -> std::unique_ptr<::hm3::vis::vtk::reader> {
  return std::make_unique<tree<nd>>();
}

/// Makes a tree grid reader from io::session block of type
/// tree.
inline auto make_tree(io::json const& b)
 -> std::unique_ptr<::hm3::vis::vtk::reader> {
  string block_type = io::read_field(b, "type", HM3_AT_);
  if (block_type != tree_type()) {
    HM3_TERMINATE("Cannot make a tree reader from a block of type \"{}\"",
                  block_type);
  }

  if (b["files"].empty()) {
    HM3_TERMINATE("tree contains no files:\n\n{}\n\n", b);
  }

  uint_t nd = io::read_file_field(b, "spatial_dimension", HM3_AT_);
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
      HM3_TERMINATE("unsupported #of spatial dimensions, nd = {}", nd);
    }
  }
}

}  // namespace grid
}  // namespace vtk
}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
