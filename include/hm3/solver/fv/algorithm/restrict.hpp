#pragma once
/// \file
///
/// Restrict cell from one tile into another tile
#include <hm3/grid/hierarchical/tree/relations/tree.hpp>

namespace hm3 {
namespace solver {
namespace fv {

struct restrict_fn {
  template <typename S, typename Tile, typename CIdx, typename VS>
  constexpr void operator()(S&& s, Tile& from, VS&& vars_from, Tile& to,
                            CIdx cell_to, VS&& vars_to) const noexcept {
    HM3_ASSERT(from.level = to.level + 1,
               "error: cannot restrict, invalid levels");

    const auto g_parent = to.geometry()(cell_to);
    vars_to(cell_to).fill(0.);

    for (auto child_pos : s.grid_client.tree().child_positions()) {
      auto x_child = child_centroid(child_pos, g_parent);
      auto child   = from.geometry().internal_cell_containing(x_child);
      HM3_ASSERT(child, "");
      vars_to(cell_to) += vars_from(child);
    }
    vars_to(cell_to) /= s.grid_client.tree().no_children();
  }
};

namespace {
static constexpr auto&& restrict = static_const<restrict_fn>::value;
}  // namespace

}  // namespace fv
}  // namespace solver
}  // namespace hm3
