#ifdef ABCD
#pragma once
/// \file
///
/// Project cell from one tile into another tile
#include <hm3/grid/hierarchical/tree/relations/neighbor.hpp>
#include <hm3/solver/fv/algorithm/central_difference.hpp>

namespace hm3::solver::fv {

struct project_fn {
  template <typename S, typename Tile, typename CIdx, typename VS,
            typename Limiter>
  constexpr void operator()(S&& s, Tile& from, CIdx&& cell_from, VS&& vars_from,
                            Tile& to, VS&& vars_to, Limiter&& limiter) const
   noexcept {
    HM3_ASSERT(from.level = to.level - 1,
               "error: cannot project, invalid levels");

    const auto g_parent = to.geometry()(cell_from);
    const auto x_parent = centroid(g_parent);

    // Compute local gradients of the parent:
    using var_t = decltype(structured_central_difference.limited(
     cell_from, vars_from, dim_t{0}, from.geometry().cell_length(), limiter));
    std::array<var_t, ad_v<Tile>> grad_parent;
    for (auto d : ambient_dimensions(s)) {
      grad_parent[d] = structured_central_difference.limited(
       cell_from, vars_from, d, from.geometry().cell_length(), limiter);
    }

    for (auto child_pos : s.grid_client.tree().child_positions()) {
      const auto x_child = child_centroid(child_pos, g_parent);
      const auto child   = to.geometry().halo_cell_containing(x_child);
      HM3_ASSERT(child, "child {} of tile must be a halo cell", child);

      vars_to(child) = vars_from(cell_from);
      for (auto d : ambient_dimensions(s)) {
        auto dx     = x_child(d) - x_parent(d);
        auto grad_x = grad_parent[d];
        vars_to(child) += grad_x * dx;
      }
    }
  }
};

namespace {
constexpr auto const& project = static_const<project_fn>::value;
}  // namespace

}  // namespace hm3::solver::fv
#endif
