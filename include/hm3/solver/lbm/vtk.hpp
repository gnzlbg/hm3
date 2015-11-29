#pragma once
/// \file
///
#include <hm3/solver/lbm/state.hpp>
#include <hm3/vis/vtk/serialize.hpp>
#include <hm3/utility/variant.hpp>
#include <hm3/geometry/square.hpp>
#include <hm3/geometry/polygon.hpp>

namespace hm3 {
namespace solver {
namespace lbm {

namespace vtk {

template <typename State>
struct serializable : dimensional<State::dimension()> {
  State const& s;
  using block_idx = grid_node_idx;
  block_idx idx   = block_idx{};

  using vtk_cell_idx         = cell_idx;
  static constexpr uint_t Nd = State::dimension();
  using geometries_t = variant<geometry::square<Nd>, geometry::polygon<Nd, 5>>;

  auto geometry(cell_idx c) const noexcept {
    return geometries_t{s.geometry(c)};
  }

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
      auto n = s.block(c);
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
    using lattice = typename std::decay_t<State>::lattice_t;
    lattice::load(s, cell_data);
  }

  serializable(State const& s_, block_idx b = block_idx{}) : s(s_), idx{b} {}
};

template <typename Ls, typename State>
struct ls_serializable : serializable<State> {
  using self = ls_serializable<Ls, State>;
  using base = serializable<State>;
  using base::s;
  using base::idx;
  using block_idx            = typename base::block_idx;
  static constexpr uint_t Nd = State::dimension();
  Ls const& ls;
  using vtk_cell_idx = cell_idx;

  using geometries_t = variant<geometry::square<Nd>, geometry::polygon<Nd, 5>>;

  geometries_t geometry(cell_idx c) const noexcept {
    geometries_t v;
    auto g = s.geometry(c);
    if (!ls.is_cut(g)) {
      v = g;
      return v;
    }
    v = std::get<0>(intersect(g, ls));
    return v;
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

  template <typename F> auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }

  ls_serializable(Ls const& l, State const& s_, block_idx b = block_idx{})
   : serializable<State>(s_, b), ls(l) {}
};

template <typename State>
void serialize(State const& state, string file_name, uint_t time_step,
               grid_node_idx b = grid_node_idx{}) {
  using std::to_string;
  hm3::log::serial log("fv-serialization-to-vtk");

  if (b) { file_name += "_block_" + to_string(b); }
  file_name += "_" + to_string(time_step);

  serializable<State> s(state, b);
  ::hm3::vis::vtk::serialize(s, file_name, log);
}

template <typename State, typename Ls>
void ls_serialize(State const& state, Ls const& ls, string file_name,
                  uint_t time_step, grid_node_idx b = grid_node_idx{}) {
  using std::to_string;
  hm3::log::serial log("fv-serialization-to-vtk");

  if (b) { file_name += "_block_" + to_string(b); }
  file_name += "_" + to_string(time_step);

  ls_serializable<Ls, State> s(ls, state, b);
  ::hm3::vis::vtk::serialize(s, file_name, log);
}

}  // namespace vtk

}  // namespace lbm
}  // namespace solver
}  // namespace hm3
