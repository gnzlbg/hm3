#pragma once
/// \file
///
/// Serialization to HM3's File I/O
#include <hm3/grid/hierarchical/adaptor/multi.hpp>
#include <hm3/grid/hierarchical/tree/serialization/fio.hpp>

namespace hm3::grid::hierarchical {
namespace adaptor {

/// Maps arrays in the file descriptor to memory addresses
template <typename TreeGrid>
void map_arrays(io::file& f, multi<TreeGrid> const& t) {
  // Support reading a number of grids smaller than grid capacity:
  grid_idx no_grids = f.has_field("no_grids")
                       ? grid_idx{f.constant("no_grids", int64_t{})}
                       : t.no_grids();

  for (auto&& g = 0_g; g != no_grids; ++g) {
    auto field_name       = "grid_" + std::to_string(*g);
    const idx_t* data_ptr = reinterpret_cast<const idx_t*>(&t.grids_(0_n, g));
    f.field(field_name, data_ptr, *t.size());
  }
}

template <typename TreeGrid>
multi<TreeGrid> from_file_unread(multi<TreeGrid> const&, io::file& f,
                                 tree_node_idx node_capacity,
                                 grid_idx grid_capacity) {
  int64_t no_grids = f.constant("no_grids", int64_t{});
  no_grids
   = grid_capacity ? *grid_capacity : f.constant("no_grids", uint64_t{});
  grid_capacity = !grid_capacity ? no_grids : grid_capacity;
  if (grid_capacity) {
    if (grid_capacity < no_grids) {
      HM3_FATAL_ERROR("The number of grids {} is smaller than in the file {}",
                      grid_capacity, no_grids);
    }
    no_grids = *grid_capacity;
  }

  multi<TreeGrid> t(grid_capacity,
                    from_file_unread(TreeGrid{}, f, node_capacity));
  map_arrays(f, t);

  // Move the hierarchical cartesian grid out of the function:
  static_assert(std::is_move_constructible<multi<TreeGrid>>{},
                "if hierarchical::cartesian::multi is not move constructible "
                "mapping the arrays fails (they will be mapped to the wrong "
                "addresses in memory)");
  return t;
}

/// Reads hierarchical::cartesian::multi from file descriptor \p f
template <typename TreeGrid>
multi<TreeGrid> from_file(multi<TreeGrid> const&, io::file& f,
                          tree_node_idx node_capacity = tree_node_idx{},
                          grid_idx grid_capacity      = grid_idx{}) {
  auto&& t
   = from_file_unread(multi<TreeGrid>{}, f, node_capacity, grid_capacity);
  f.read_arrays();
  return t;
}

/// Appends constants and map arrays to file \p f
template <typename TreeGrid>
void to_file_unwritten(io::file& f, multi<TreeGrid> const& t) {
  to_file_unwritten(f, static_cast<TreeGrid const&>(t));
  f.field("no_grids", *t.no_grids());
  map_arrays(f, t);
}

}  // namespace adaptor
}  // namespace hm3::grid::hierarchical
