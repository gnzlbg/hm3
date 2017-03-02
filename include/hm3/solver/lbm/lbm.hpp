#pragma once
/// \file
///
///
#include <hm3/types.hpp>
#include <hm3/utility/fmt.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace solver {
namespace lbm {

/// Initializes variables to the equilibrium distribution
template <typename State, typename InitialCondition>
void initialize_variables_to_equilibrium(State&& s, InitialCondition&& ic) {
  for (auto&& b : s.blocks()) {
    b.for_each_internal([&](auto&& c) {
      auto x    = b.centroid(c);
      auto f_eq = s.physics.equilibrium_distribution(ic(x));
      for (auto&& d : s.physics.all()) {
        b.nodes0(c, d) = f_eq[d];
        b.nodes1(c, d) = f_eq[d];
      }
    });
  }
}

template <typename State, typename BCs>
void set_boundary(State&& s, BCs&& bcs) {
  for (auto&& b : s.blocks()) {
    b.for_each([&](auto&& c) {
      b.is_boundary(c) = false;
      ranges::tuple_for_each(bcs, [&](auto&& bc) {
        b.is_boundary(c) = b.is_boundary(c) or bc(s, b, c);
      });
    });
  }
}

// Single Relaxation Time
template <typename Distributions, typename Physics>
static constexpr void collide(Distributions&& from, Distributions&& to,
                              Physics&& physics, num_t omega) noexcept {
  auto f_eq = physics.equilibrium_distribution(from);
  for (auto&& d : physics.all()) {
    to[d] = from[d] + omega * (f_eq[d] - from[d]);
  }
}

template <typename State, typename Solid>
void collide(State&& s, Solid&& solid, num_t omega) noexcept {
  // num_t omega
  //  = physics.omega(static_cast<num_t>(max_level), static_cast<num_t>(level));
  // num_t omega = 0.1;
  ascii_fmt::out("omega: {}", omega);
  for (auto&& b : s.blocks()) {
    b.for_each_internal([&](auto&& c) {
      if (solid(s, b, c)) { return; }
      collide(&b.nodes1(c, 0), &b.nodes0(c, 0), s.physics, omega);
    });
  }
}

template <typename State>
void propagate(State&& s) noexcept {
  for (auto&& b : s.blocks()) {
    b.for_each_internal([&](auto&& c) {
      for (auto&& d : s.physics.all()) {
        auto nghbr = b.at(c, s.physics.dir(d));
        b.nodes1(nghbr, d) = b.nodes0(c, d);
      }
    });
  }
}
template <typename State, typename Solid>
void propagate_periodic_x(State&& s, Solid&& solid) noexcept {
  for (auto&& b : s.blocks()) {
    b.for_each_halo([&](auto&& c) {
      if (solid(s, b, c)) { return; }
      for (auto&& d : s.physics.all()) {
        auto nghbr = b.at(c, s.physics.periodic_neighbor_dir_x(d));
        b.nodes1(nghbr, d) = b.nodes0(c, d);
      }
    });
  }
}

template <typename State, typename Solid>
void propagate_slip(State&& s, Solid&& solid) noexcept {
  for (auto&& b : s.blocks()) {
    b.for_each_halo([&](auto&& c) {
      if (solid(s, b, c)) { return; }
      for (auto&& d : s.physics.all()) {
        auto nghbr = b.at(c, s.physics.dir(d));
        if (solid(s, b, nghbr)) { continue; }
        b.nodes1(nghbr, d) = b.nodes0(c, d);
      }
    });
  }
}

// template <typename State, typename BCs>
// void propagate(State&& s, BCs&& bcs) noexcept {
//   using l = typename uncvref_t<State>::lattice_t;
//   for (auto&& b : s.blocks()) {
//     b.for_each_internal([&](auto&& c) {
//       for (auto&& d : l::node_ids_without_center()) {
//         auto nghbr = b.at(c, l::neighbor_offset(d));
//         if (b.is_halo(nghbr)) { continue; }
//         // if (nghbr.x[1] == 2 or nghbr.x[1] == 101 or b.is_halo(nghbr)
//         //     or bcs(b.centroid(nghbr)) < 0.) {
//         //   continue;
//         // }
//         b.nodes1(nghbr, d) = b.nodes0(c, d);
//       }
//     });
//   }
// }

// template <typename State> num_t compute_total_density(State&& s) {
//   using l   = typename uncvref_t<State>::lattice_t;
//   num_t rho = 0.;
//   for (auto&& b : s.blocks()) {
//     num_t rho_l = 0.;
//     b.for_each_internal([&](auto&& c) { rho_l += l::rho(&b.nodes0(c, 0)); });
//     rho += rho_l;  // atomic
//   }
//   return rho;
// }

// template <typename State, typename BCs>
// num_t advance(State&& s, BCs&& bcs, uint_t no_iterations = 1) {
//   num_t d   = 0.;
//   int count = 1;
//   while (no_iterations > 0) {
//     bcs.redistribute(s);
//     propagate(s, bcs);
//     bcs.periodic(s);
//     bcs.bounce_back(s);
//     collide(s, bcs);
//     d = compute_total_density(s);
//     std::cout << "density: " << d << std::endl;
//     // vtk::ls_serialize(s, bcs, "lbm_res", count);
//     vtk::serialize(s, "lbm_res", count);
//     --no_iterations;
//     ++count;
//   }
//   return d;
// }

}  // namespace lbm
}  // namespace solver
}  // namespace hm3
