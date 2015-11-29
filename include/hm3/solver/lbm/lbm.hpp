#pragma once
/// \file lbm.hpp

namespace hm3 {
namespace solver {
namespace lbm {

template <typename State> void init_variables(State&& s, num_t density) {
  num_t t_0 = density * 4. / 9.;
  num_t t_1 = density / 9.;
  num_t t_2 = density / 36.;
  using l   = typename std::decay_t<State>::lattice_t;

  for (auto&& b : s.blocks()) {
    b.for_each([&](auto&& c) {
      b.nodes0(c, l::center_node_id()) = t_0;
      RANGES_FOR (auto&& d, l::direct_node_ids()) { b.nodes0(c, d) = t_1; }
      RANGES_FOR (auto&& d, l::diagonal_node_ids()) { b.nodes0(c, d) = t_2; }
    });
  }
}

template <typename State, typename BCs>
void propagate(State&& s, BCs&& bcs) noexcept {
  using l = typename std::decay_t<State>::lattice_t;
  for (auto&& b : s.blocks()) {
    b.for_each_internal([&](auto&& c) {
      RANGES_FOR (auto&& d, l::node_ids_without_center()) {
        auto nghbr = b.at(c, l::neighbor_offset(d));
        if (b.is_halo(nghbr)) { continue; }
        // if (nghbr.x[1] == 2 or nghbr.x[1] == 101 or b.is_halo(nghbr)
        //     or bcs(b.center(nghbr)) < 0.) {
        //   continue;
        // }
        b.nodes1(nghbr, d) = b.nodes0(c, d);
      }
    });
  }
}

template <typename State, typename BCs>
void collide(State&& s, BCs&& bcs) noexcept {
  using l = typename std::decay_t<State>::lattice_t;
  for (auto&& b : s.blocks()) {
    b.for_each_internal([&](auto&& c) {
      auto x = b.center(c);
      if (c.x[1] == 2 || c.x[1] == 101 || bcs(x) < 0.) { return; }
      l::collide(&b.nodes1(c, 0), &b.nodes0(c, 0), 1.85, 1. / 3.);
    });
  }
}

template <typename State> num_t compute_total_density(State&& s) {
  using l   = typename std::decay_t<State>::lattice_t;
  num_t rho = 0.;
  for (auto&& b : s.blocks()) {
    num_t rho_l = 0.;
    b.for_each_internal([&](auto&& c) { rho_l += l::rho(&b.nodes0(c, 0)); });
    rho += rho_l;  // atomic
  }
  return rho;
}

template <typename State, typename BCs>  //
num_t advance(State&& s, BCs&& bcs, uint_t no_iterations = 1) {
  num_t d   = 0.;
  int count = 1;
  while (no_iterations > 0) {
    bcs.redistribute(s);
    propagate(s, bcs);
    bcs.periodic(s);
    bcs.bounce_back(s);
    collide(s, bcs);
    d = compute_total_density(s);
    std::cout << "density: " << d << std::endl;
    // vtk::ls_serialize(s, bcs, "lbm_res", count);
    vtk::serialize(s, "lbm_res", count);
    --no_iterations;
    ++count;
  }
  return d;
}

}  // namespace lbm
}  // namespace solver
}  // namespace hm3
