#pragma once
/// \file
///
/// Serialization of FV solver to VTK
#ifdef HM3_ENABLE_VTK
#include <hm3/geometry/polygon.hpp>
#include <hm3/geometry/polygon/intersection.hpp>
#include <hm3/geometry/square.hpp>
#include <hm3/solver/fv/state.hpp>
#include <hm3/utility/variant.hpp>
#include <hm3/vis/vtk/geometry.hpp>
#include <hm3/vis/vtk/serialize.hpp>

namespace hm3 {
namespace solver {
namespace fv {

namespace vtk {

template <typename State, typename T>
struct serializable : geometry::dimensional<State::dimension()> {
  State const& s;
  using block_idx = grid_node_idx;
  block_idx idx   = block_idx{};

  using vtk_cell_idx = cell_idx;

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

  template <typename F> auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }

  template <typename CellData> void load(CellData&& cell_data) const {
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
    if (idx) {
      cell_data.load("is_internal", [&](cell_idx c, auto&&) {
        return static_cast<int_t>(s.is_internal(c));
      });
    }
    s.physics.load(T{s.physics}, s, cell_data);
  }

  serializable(State const& s_, block_idx b = block_idx{}) : s(s_), idx{b} {}
};

template <typename Ls, typename State, typename T>
struct ls_serializable : serializable<State, T> {
  using self = ls_serializable<Ls, State, T>;
  using base = serializable<State, T>;
  using base::s;
  using base::idx;
  using block_idx           = typename base::block_idx;
  static constexpr dim_t Nd = State::dimension();
  Ls const& ls;
  using vtk_cell_idx = cell_idx;

  vis::vtk::geometries<Nd> geometry(cell_idx c) const noexcept {
    auto g = s.geometry(c);
    if (!geometry::is_intersected(g, ls)) { return g; }
    return std::get<0>(intersect(g, ls));
  }

  auto nodes() const noexcept {
    return s.all_cells() | view::filter([&](cell_idx c) {
             if (!idx) {
               return s.is_internal(c)
                      and (geometry::is_completely_inside(s.geometry(c), ls)
                           or geometry::is_intersected(s.geometry(c), ls));
             } else {
               return s.is_in_block(c, idx);
             }
           });
  }

  template <typename F> auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }

  template <typename CellData> void load(CellData&& cell_data) const {
    serializable<State, T>::load(cell_data);
    cell_data.load("level_set", [&](cell_idx n, auto&&) {
      auto x_i = geometry::centroid(s.geometry(n));
      return ls(x_i);
    });
    s.physics.load(T{s.physics}, s, cell_data);
  }

  ls_serializable(Ls const& l, State const& s_, block_idx b = block_idx{})
   : serializable<State, T>(s_, b), ls(l) {}
};

template <typename State, typename T>
void serialize(State const& state, string file_name, uint_t time_step, T&&,
               grid_node_idx b = grid_node_idx{}) {
  using std::to_string;
  hm3::log::serial log("fv-serialization-to-vtk");

  if (b) { file_name += "_block_" + to_string(b); }
  file_name += "_" + to_string(time_step);

  serializable<State, T> s(state, b);
  ::hm3::vis::vtk::serialize(s, file_name, log);
}

template <typename State, typename Ls, typename T>
void ls_serialize(State const& state, Ls const& ls, string file_name,
                  uint_t time_step, T&&, grid_node_idx b = grid_node_idx{}) {
  using std::to_string;
  hm3::log::serial log("fv-serialization-to-vtk");

  if (b) { file_name += "_block_" + to_string(b); }
  file_name += "_" + to_string(time_step);

  ls_serializable<Ls, State, T> s(ls, state, b);
  ::hm3::vis::vtk::serialize(s, file_name, log);
}

}  // namespace vtk

}  // namespace fv
}  // namespace solver
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
