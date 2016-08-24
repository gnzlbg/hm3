#ifdef FIXED
#pragma once
/// \file
///
/// Serialization of LBM solver to VTK
#ifdef HM3_ENABLE_VTK
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/solver/lbm/state.hpp>
#include <hm3/utility/variant.hpp>
#include <hm3/vis/vtk/geometry.hpp>
#include <hm3/vis/vtk/serialize.hpp>

namespace hm3 {
namespace solver {
namespace lbm {

namespace vtk {

template <typename State, typename Solid>
struct serializable : geometry::dimensional<State::dimension()> {
  State const& s;
  Solid const& solid;
  using block_idx = grid_node_idx;
  block_idx idx   = block_idx{};

  using vtk_cell_idx        = cell_idx;
  static constexpr dim_t Nd = State::dimension();

  auto geometry(cell_idx c) const noexcept { return s.geometry(c); }

  auto nodes() const noexcept {
    return s.all_cells() | view::filter([&](cell_idx c) {
             if (!idx) {
               return s.is_internal(c);
             } else {
               return s.is_in_block(c, idx);
             }
           });
  }

  auto bounding_box() const noexcept { return s.grid.tree().bounding_box(); }

  template <typename F>
  auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }

  template <typename CellData>
  void load(CellData&& cell_data) const {
    cell_data.load("cell_idx", [&](cell_idx n, auto&&) {
      return n ? static_cast<int_t>(*n) : int_t{-1};
    });
    cell_data.load("block_idx", [&](cell_idx c, auto&&) {
      auto n = s.block_i(c);
      return n ? static_cast<int_t>(*n) : int_t{-1};
    });
    cell_data.load("block_cell_idx", [&](cell_idx c, auto&&) {
      auto n = s.block_cell_idx(c);
      return n;  // ? static_cast<int_t>(*n) : int_t{-1};
    });
    cell_data.load("solid", [&](cell_idx c, auto&&) {
      auto n   = s.block_cell_idx(c);
      auto&& b = s.block(s.block_i(c));
      return int_t{
       solid(s, b, b.from(n))};  // ? static_cast<int_t>(*n) : int_t{-1};
    });

    if (idx) {
      cell_data.load("is_internal", [&](cell_idx c, auto&&) {
        return static_cast<int_t>(s.is_internal(c));
      });
    }
    s.physics.load(s, cell_data);
  }

  serializable(State const& s_, Solid const& sol, block_idx b = block_idx{})
   : s(s_), solid(sol), idx{b} {}
};

template <typename Ls, typename State, typename Solid>
struct ls_serializable : serializable<State, Solid> {
  using self = ls_serializable<Ls, State, Solid>;
  using base = serializable<State, Solid>;
  using base::s;
  using base::idx;
  using block_idx           = typename base::block_idx;
  static constexpr dim_t Nd = State::dimension();
  Ls const& ls;
  using vtk_cell_idx = cell_idx;

  vis::vtk::geometries<Nd> geometry(cell_idx c) const noexcept {
    auto g = s.geometry(c);
    if (!ls.is_cut(g)) { return g; }
    return std::get<0>(intersect(g, ls));
  }

  auto nodes() const noexcept {
    return s.all_cells() | view::filter([&](cell_idx c) {
             if (!idx) {
               return s.is_internal(c) and (ls.is_inside(s.geometry(c))
                                            or ls.is_cut(s.geometry(c)));
             } else {
               return s.is_in_block(c, idx);
             }
           });
  }

  template <typename F>
  auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }

  ls_serializable(Ls const& l, State const& s_, Solid const& sol,
                  block_idx b = block_idx{})
   : serializable<State, Solid>(s_, b, sol), ls(l) {}
};

template <typename State, typename Solid>
void serialize(State const& state, Solid const& solid, string file_name,
               uint_t time_step, grid_node_idx b = grid_node_idx{}) {
  using std::to_string;
  hm3::log::serial log("fv-serialization-to-vtk");

  if (b) { file_name += "_block_" + to_string(b); }
  file_name += "_" + to_string(time_step);

  serializable<State, Solid> s(state, solid, b);
  ::hm3::vis::vtk::serialize(s, file_name, log);
}

template <typename State, typename Solid, typename Ls>
void ls_serialize(State const& state, Solid const& solid, Ls const& ls,
                  string file_name, uint_t time_step,
                  grid_node_idx b = grid_node_idx{}) {
  using std::to_string;
  hm3::log::serial log("fv-serialization-to-vtk");

  if (b) { file_name += "_block_" + to_string(b); }
  file_name += "_" + to_string(time_step);

  ls_serializable<Ls, State, Solid> s(ls, state, solid, b);
  ::hm3::vis::vtk::serialize(s, file_name, log);
}

}  // namespace vtk

}  // namespace lbm
}  // namespace solver
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
#endif
