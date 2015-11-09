#pragma once
/// \file
///
/// Finite Volume solver
#include <hm3/solver/fv/state.hpp>

namespace hm3 {
namespace solver {
namespace fv {

/// Clears the Right Hand Side of all blocks
template <typename Physics> void clear_rhs(state<Physics>& s) {
  for (auto&& b : s.blocks()) { b.rhs_().fill(0.); }
  for (auto&& b : s.blocks()) { b.fluxes_().fill(0.); }
  for (auto&& b : s.blocks()) { b.gradients_().fill(0.); }
}

/// Clears the Halo cells of all blocks
///
/// TODO: enable only in debug mode (their values should always be overwritten)
template <typename Physics> void clear_halos(state<Physics>& s) {
  for (auto&& b : s.blocks()) {
    b.for_each_halo([&](auto&& c) {
      b.variables(c.idx).fill(0.);
      // flux
      // gradient
      //
    });
  }
}

inline num_t compute_gradient(num_t vL, num_t vR, num_t distance) noexcept {
  return (vR - vL) / distance;
}

template <typename LeftVars, typename RightVars>
inline decltype(auto) compute_gradient(LeftVars&& vL, RightVars&& vR,
                                       num_t const& distance) noexcept {
  return (vR - vL) / distance;
}

template <typename Physics, typename block_t, typename Idx,
          CONCEPT_REQUIRES_(Same<typename state<Physics>::block_t, block_t>{})>
void compute_gradients(state<Physics>& s, block_t& b, Idx c) {
  for (auto d : b.dimensions()) {
    auto cp1 = b.at(c, d, +1);
    auto cm1 = b.at(c, d, -1);
    // std::cerr << "idx: " << c.idx << " cp1: " << cp1.idx << " cm1: " <<
    // cm1.idx
    //           << std::endl;
    b.gradient(c, d) = compute_gradient(b.variables(cp1), b.variables(cm1),
                                        2. * b.cell_length());
  }
}

template <typename Physics> void compute_gradients(state<Physics>& s) {
  // std::cerr << "compute gradients: " << std::endl;
  int count_ = 0;
  for (auto&& b : s.blocks()) {
    // std::cout << "block: " << count_ << std::endl;
    b.for_each_internal([&](auto c) { compute_gradients(s, b, c); }, 1);
    count_++;
  }
}

template <typename Physics> void exchange_halos(state<Physics>& s) {
  // Iterate over all blocks and perform copies and restrictions (which are cell
  // local)
  for (auto&& bidx : s.block_grid()) {
    auto&& b = s.block(bidx);
    for (auto bn_idx : s.block_neighbors(bidx)) {
      auto&& bn = s.block(bn_idx);
      // If the level is the same: copy neighbor cells into block halos:
      if (b.block_level == bn.block_level) {
        b.for_each_halo([&](auto hc) {
          auto x_hc = b.center(hc);
          if (!b.in_block(x_hc)) { return; }
          auto nc = bn.at(x_hc);

          for (auto v : s.physics.variables()) {
            b.variable(hc.idx, v) = bn.variable(nc.idx, v);
          }
        });
      }
      if (b.block_level < bn.block_level) {
        HM3_FATAL_ERROR("unsupported b.block_level {} < bn.block_level {}",
                        b.block_level, bn.block_level);
        // // if this block level is smaller, restrict from neighbor cells
        // into
        // // block
        // // halos
        // if (b.block_level < bn.block_level) {
        //   bn.for_each_internal([&](auto nic) {
        //     auto x_nc + bn.center(nic);
        //     auto hc = b.at(x_nc);
        //     if (!hc) { return; }
        //     for (auto v : s.physics.variables()) {
        //       // restriction:
        //       b.variable(hc, v)
        //        = bn.variable(nic, v) / tree::no_children(s.dimension());
        //     }
        //   });
        // }
      }
      if (b.block_level > bn.block_level) {
        HM3_FATAL_ERROR("unsupported b.block_level {} > bn.block_level {}",
                        b.block_level, bn.block_level);
      }
    }

    // /// Iterate over all blocks and perform projects (which requires
    // gradients
    // /// on
    // /// the parent cell, which can only be computed locally if the previous
    // step
    // /// was completed and the blocks and lower levels have already been
    // /// projected)
    // for (auto l : s.levels()) {
    //   for (auto&& bidx : s.block_ids()) {
    //     if (s.block(bidx).block_level != l) { continue; }
    //     auto&& b = s.block(bidx);
    //     for (auto bn_idx : s.neighbors(bidx)) {
    //       auto&& bn = s.block(bn_idx);
    //       // If this blocks level is higher, project from neighbor cells
    //       into
    //       // block halos
    //       if (b.block_level > bn.block_level) {
    //         n.for_each_halo([&](auto hc) {
    //           auto x_hc + b.center(hc);
    //           auto nc = bn.at(x_hc);
    //           if (!nc) { return; }
    //           fv::compute_gradients(s, bn, nc);  // gradients of neighbor
    //           fv::project(bn, nc, b, hc);        // from neighbor to halo
    //         });
    //       }
    //     }
    //   }
    // }
  }
}

template <typename Physics, typename TimeStepF, typename V>
num_t compute_time_step(state<Physics> const& s, num_t cfl, TimeStepF&& t,
                        V&& v) {
  num_t dt = std::numeric_limits<num_t>::max();
  for (auto&& b : s.blocks()) {
    b.for_each_internal([&](auto c) {
      num_t idt = t(b.variables(c), v, b.cell_length(), cfl);
      dt = std::min(dt, idt);
    });
  }
  return dt;
}

template <typename Physics> void cv_to_pv(state<Physics>& s) {
  for (auto&& b : s.blocks()) {
    b.for_each([&](auto&& c) { s.physics.cv_to_pv(b.variables(c)); });
  }
}

template <typename Physics> void pv_to_cv(state<Physics>& s) {
  for (auto&& b : s.blocks()) {
    b.for_each([&](auto&& c) { s.physics.pv_to_cv(b.variables(c)); });
  }
}

template <typename Physics, typename NumFluxF, typename V>
void compute_internal_fluxes(state<Physics>& s, NumFluxF&& nf, num_t dt,
                             V&& v) {
  for (auto&& b : s.blocks()) {
    struct {
      num_t dx, area, volume, dt;
    } data{b.cell_length(), b.cell_surface_area(), b.cell_volume(), dt};
    b.for_each_internal([&](auto c) {
      for (auto&& d : b.dimensions()) {
        b.flux(c, d * 2)
         = nf(v, b.variables(b.at(c, d, -1)), b.variables(c), d, data);
        b.flux(c, d * 2 + 1)
         = nf(v, b.variables(c), b.variables(b.at(c, d, +1)), d, data);
      }
    });

    // b.for_each_internal([&](auto c) {
    //   if (c.idx < 3000 || c.idx > 4000) { return; }
    //   for (auto&& d : b.dimensions()) {
    //     std::cout << "c: " << c.idx << " f(-1): " << b.flux(c, d * 2)
    //               << " | f(+1): " << b.flux(c, d * 2 + 1) << std::endl;
    //   }
    // });
  }
}

/// Updates RHS with fluxes
template <typename Physics> void update_rhs(state<Physics>& s) {
  for (auto&& b : s.blocks()) {
    b.for_each_internal([&](auto c) {
      for (auto&& d : b.dimensions()) {
        b.rhs(c) += b.flux(c, d * 2);
        b.rhs(c) -= b.flux(c, d * 2 + 1);
      }
    });
  }
}

/// Updates variables with RHS
template <typename Physics>
void advance_variables(state<Physics>& s, num_t dt) {
  for (auto&& b : s.blocks()) {
    const num_t f = dt / b.cell_length();
    std::cerr << "cl: " << b.cell_length() << " f: " << f << std::endl;

    b.for_each_internal([&](auto c) {
      b.variables(c) += dt * b.rhs(c);
      // b.variables(c) += dt * b.rhs(c);
    });
  }
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
