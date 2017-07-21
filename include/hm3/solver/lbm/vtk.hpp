#pragma once
/// \file
///
/// Serialization of LBM solver to VTK
#ifdef HM3_ENABLE_VTK
#include <hm3/ext/variant.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/solver/lbm/state.hpp>
#include <hm3/vis/vtk/serialize.hpp>

namespace hm3::solver::lbm::vtk {

template <typename State>
struct serializable : hg::with_ambient_dimension<State::ambient_dimension()> {
  State const& s;
  using tile_idx = grid_node_idx;
  tile_idx idx   = tile_idx{};

  using vtk_cell_idx = cell_idx_t;

  auto geometry(cell_idx_t c) const noexcept { return s.geometry(c); }

  auto nodes() const noexcept {
    return s.all_cells() | view::filter([&](cell_idx_t c) {
             if (!idx) {
               return s.is_internal(c);
             } else {
               return s.is_in_tile(c, idx);
             }
           });
  }

  auto bounding_box() const noexcept { return s.tree().bounding_box(); }

  template <typename F>
  auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }

  template <typename CellData>
  void load(CellData&& cell_data) const {
    cell_data.load("cell_idx", [&](cell_idx_t n, auto&&) {
      return n ? static_cast<int_t>(*n) : int_t{-1};
    });
    cell_data.load("tile_idx", [&](cell_idx_t c, auto&&) {
      auto n = s.tile_idx(c);
      return n ? static_cast<int_t>(*n) : int_t{-1};
    });
    cell_data.load("tile_local_cell_idx", [&](cell_idx_t c, auto&&) {
      auto n = *s.tile_local_cell_idx(c);
      return n;  // ? static_cast<int_t>(*n) : int_t{-1};
    });

    if (idx) {
      cell_data.load("is_internal", [&](cell_idx_t c, auto&&) {
        return static_cast<int_t>(s.is_internal(c));
      });
    }
    s.physics.load(s, cell_data);
  }

  serializable(State const& s_, tile_idx b = tile_idx{}) : s(s_), idx{b} {}
};

template <typename Ls, typename State>
struct ls_serializable : serializable<State> {
  using self = ls_serializable<Ls, State>;
  using base = serializable<State>;
  using base::s;
  using base::idx;
  using tile_idx            = typename base::tile_idx;
  static constexpr dim_t Ad = State::ambient_dimension();
  Ls const& ls;
  using vtk_cell_idx = cell_idx_t;

  vis::vtk::geometry_t<Ad> geometry(cell_idx_t c) const noexcept {
    auto g = s.geometry(c);
    if (!ls.is_cut(g)) { return g; }
    return std::get<0>(intersect(g, ls));
  }

  auto nodes() const noexcept {
    return s.all_cells() | view::filter([&](cell_idx_t c) {
             if (!idx) {
               return s.is_internal(c)
                      and (ls.is_inside(s.geometry(c))
                           or ls.is_cut(s.geometry(c)));
             } else {
               return s.is_in_tile(c, idx);
             }
           });
  }

  template <typename F>
  auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }

  ls_serializable(Ls const& l, State const& s_, tile_idx b = tile_idx{})
   : serializable<State>(s_, b), ls(l) {}
};

template <typename State>
void serialize(State const& state, string file_name, uint_t time_step,
               grid_node_idx b = grid_node_idx{}) {
  using std::to_string;
  hm3::log::serial log("lbm-serialization-to-vtk");

  if (b) { file_name += "_tile_" + to_string(b); }
  file_name += "_" + to_string(time_step);

  serializable<State> s(state, b);
  ::hm3::vis::vtk::serialize(s, file_name, log);
}

template <typename State, typename Ls>
void ls_serialize(State const& state, Ls const& ls, string file_name,
                  uint_t time_step, grid_node_idx b = grid_node_idx{}) {
  using std::to_string;
  hm3::log::serial log("lbm-serialization-to-vtk");

  if (b) { file_name += "_tile_" + to_string(b); }
  file_name += "_" + to_string(time_step);

  ls_serializable<Ls, State> s(ls, state, b);
  ::hm3::vis::vtk::serialize(s, file_name, log);
}

}  // namespace hm3::solver::lbm::vtk
#endif  // HM3_ENABLE_VTK
