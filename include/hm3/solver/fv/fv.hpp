#pragma once
/// \file
///
/// Finite Volume solver
#include <hm3/solver/fv/gradient.hpp>
#include <hm3/solver/fv/limiters.hpp>
#include <hm3/solver/fv/state.hpp>

namespace hm3 {
namespace solver {
namespace fv {

/// Clears the Right Hand Side of all blocks
template <typename Physics> void clear_rhs(state<Physics>& s) {
  for (auto&& b : s.blocks()) { b.rhs()().fill(0.); }
  // for (auto&& b : s.blocks()) { b.fluxes_().fill(0.); }
  // for (auto&& b : s.blocks()) { b.gradients_().fill(0.); }
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

template <typename State, typename Limiter = limiter::none_fn>
void compute_structured_gradients(State& s, Limiter limiter = Limiter{}) {
  using vars = num_a<std::decay_t<State>::nvars()>;
  for (auto&& b : s.blocks()) {
    const auto dx  = b.cell_length();
    const auto dx2 = 2. * dx;
    b.for_each_internal(
     [&](auto c) {
       for (auto d : b.dimensions()) {
         auto cm = b.at(c, d, -1);
         auto cp = b.at(c, d, +1);

         auto vM = b.variables(cm);
         auto vP = b.variables(cp);

         if (Same<Limiter, limiter::none_fn>{}) {  // unlimited
           b.gradient(c, d) = gradient(vP, vM, dx2);
         } else {  // limited:
           auto vC = b.variables(c);

           vars gM  = gradient(vC, vM, dx);
           vars gP  = gradient(vP, vC, dx);
           vars gC  = gradient(vP, vM, dx2);
           vars lim = limiter(gM, gP);

           b.gradient(c, d) = gC.array() * lim.array();
         }
       }
     },
     1);
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
    auto dx = b.cell_length();
    b.for_each_internal(
     [&](auto c) { dt = std::min(dt, t(b.variables(c), v, dx, cfl)); });
  }
  return dt;
}

template <typename Physics, typename TimeStepF, typename V>
num_t compute_time_step(state<Physics> const& s, num_t cfl, TimeStepF&& t,
                        V&& v, num_t time, num_t time_end) {
  num_t dt = compute_time_step(s, cfl, t, v);
  if (time + dt > time_end) { dt = time_end - time; }
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

template <typename V, typename G> auto variables_at(V&& v, G&& g, num_t dx) {
  return v + g * dx;
}

template <typename NumFluxF, typename V, typename Block, typename CIdx>
num_a<std::decay_t<Block>::nvars()> structured_numerical_flux(NumFluxF&& nf,
                                                              num_t dt, V&& v,
                                                              Block&& b,
                                                              CIdx&& c) {
  num_a<4> result = num_a<4>::Zero();
  struct {
    num_t dx, area, volume, dt;
  } data;
  data.dx         = b.cell_length();
  data.area       = b.cell_surface_area();
  data.volume     = b.cell_volume();
  data.dt         = dt;
  const num_t f   = dt / data.dx;
  const num_t dx2 = data.dx / 2.;
  for (auto&& d : b.dimensions()) {
    auto cM = b.at(c, d, -1);
    auto cP = b.at(c, d, +1);

#define SECOND_ORDER
#ifdef SECOND_ORDER
    auto vM = variables_at(b.variables(cM), b.gradient(cM, d), +dx2);
    auto vcM = variables_at(b.variables(c), b.gradient(c, d), -dx2);
    auto vcP = variables_at(b.variables(c), b.gradient(c, d), +dx2);
    auto vP = variables_at(b.variables(cP), b.gradient(cP, d), -dx2);
#else
    auto vM  = b.variables(cM);
    auto vcM = b.variables(c);
    auto vcP = b.variables(c);
    auto vP  = b.variables(cP);
#endif
    result += (nf(v, vM, vcM, d, data) - nf(v, vcP, vP, d, data));
  }
  return f * result;
}

template <typename State, typename NumFluxF, typename VT>
void compute_structured_rhs(State&& s, NumFluxF&& nf, num_t dt, VT&& vt) {
  for (auto&& b : s.blocks()) {
    b.for_each_internal(
     [&](auto c) { b.rhs(c) += structured_numerical_flux(nf, dt, vt, b, c); });
  }
}

template <typename State> void advance_rhs(State& s) {
  for (auto&& b : s.blocks()) {
    b.for_each_internal([&](auto c) { b.variables(c) += b.rhs(c); });
  }
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
