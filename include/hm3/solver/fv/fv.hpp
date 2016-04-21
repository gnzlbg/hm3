#pragma once
/// \file
///
/// Finite Volume solver
#include <hm3/solver/fv/gradient.hpp>
#include <hm3/solver/fv/limiters.hpp>
#include <hm3/solver/fv/state.hpp>
#include <hm3/solver/fv/vtk.hpp>

namespace hm3 {
namespace solver {
namespace fv {

/// Clears the Right Hand Side of all blocks
template <typename State> void clear_rhs(State& s) {
  for (auto&& b : s.blocks()) { b.rhs()().fill(0.); }
  // for (auto&& b : s.blocks()) { b.fluxes_().fill(0.); }
  // for (auto&& b : s.blocks()) { b.gradients_().fill(0.); }
}

/// Clears the Halo cells of all blocks
///
/// TODO: enable only in debug mode (their values should always be overwritten)
template <typename State> void clear_halos(State& s) {
  for (auto&& b : s.blocks()) {
    b.for_each_halo([&](auto&& c) {
      b.variables(c.idx).fill(0.);
      // flux
      // gradient
      //
    });
  }
}

template <typename State, typename Block, typename CIdx, typename Limiter>
auto compute_structured_gradient(State& s, Block& b, CIdx c, dim_t d,
                                 Limiter& limiter) {
  using vars = num_a<std::decay_t<State>::nvars()>;
  vars grad;
  const auto dx  = b.cell_length();
  const auto dx2 = 2. * dx;
  auto&& lhs     = s.time_integration.lhs(b);
  auto cm        = b.at(c, d, -1);
  auto cp        = b.at(c, d, +1);

  auto vM = lhs(cm);
  auto vP = lhs(cp);

  if (Same<Limiter, limiter::none_fn>{}) {  // unlimited

    b.gradient(c, d) = gradient(vM, vP, dx2);
  } else {  // limited:
    auto vC = lhs(c);

    // vars gM  = gradient(vC, vM, dx);
    // vars gP  = gradient(vP, vC, dx);
    // vars gC  = gradient(vP, vM, dx2);

    vars gM = gradient(vM, vC, dx);
    vars gP = gradient(vC, vP, dx);
    vars gC = gradient(vM, vP, dx2);

    vars lim = limiter(gM, gP);

    grad = gC.array() * lim.array();
  }
  return grad;
}

template <typename State, typename Limiter = limiter::none_fn>
void compute_structured_gradients(State& s, Limiter limiter = Limiter{}) {
  for (auto&& b : s.blocks()) {
    b.for_each_internal(
     [&](auto c) {
       for (auto d : b.dimensions()) {
         b.gradient(c, d) = compute_structured_gradient(s, b, c, d, limiter);
       }
     },
     1);
  }
}

template <typename State, typename Limiter>
void exchange_halos(State& s, Limiter&& lim) {
  // auto lim = limiter::van_albada;
  // Iterate over all blocks and perform copies and restrictions (which are cell
  // local)
  for (auto&& bidx : s.grid()) {
    auto&& b = s.block(bidx);
    for (auto bn_idx : s.neighbors(bidx)) {
      auto&& bn    = s.block(bn_idx);
      auto&& b_lhs = s.time_integration.lhs(b);
      // If the level is the same: copy neighbor cells into block halos:
      if (b.block_level == bn.block_level) {
        b.for_each_halo([&](auto hc) {
          auto x_hc = b.centroid(hc);
          if (!bn.in_grid(x_hc)) { return; }
          auto nc = bn.at_nh(x_hc);

          b_lhs(hc) = s.time_integration.lhs(bn)(nc);
        });
      }

      // if this block level is higher, project from neighbor cells into
      // this block halos
      //
      // TODO: see below, this should be performed later
      if (b.block_level > bn.block_level) {
        b.for_each_halo([&](auto hc) {
          auto x_hc = b.centroid(hc);
          if (!bn.in_grid(x_hc)) { return; }
          auto nc   = bn.at_nh(x_hc);
          b_lhs(hc) = s.time_integration.lhs(bn)(nc);
        });
      }

      // if this block level is smaller, restrict from neighbor cells into this
      // block halos
      if (b.block_level < bn.block_level) {
        // zero the variables
        b.for_each_halo([&](auto hc) {
          auto x_hc = b.centroid(hc);
          if (!bn.in_grid(x_hc)) { return; }
          RANGES_FOR (auto&& v, s.variables()) { b_lhs(hc)(v) = 0.; }
        });
        // zero the variables
        b.for_each_halo([&](auto hc) {
          auto x_hc = b.centroid(hc);
          if (!bn.in_grid(x_hc)) { return; }
          auto nc = bn.at_nh(x_hc);
          b_lhs(hc) += s.time_integration.lhs(bn)(nc);
        });
      }
    }
  }

  for (auto&& bidx : s.grid()) {
    auto&& b = s.block(bidx);
    for (auto bn_idx : s.neighbors(bidx)) {
      auto&& bn    = s.block(bn_idx);
      auto&& b_lhs = s.time_integration.lhs(b);

      if (b.block_level > bn.block_level) {
        b.for_each_halo([&](auto hc) {
          auto x_hc = b.centroid(hc);
          if (!bn.in_grid(x_hc)) { return; }
          auto nc   = bn.at_nh(x_hc);
          auto x_nc = bn.centroid(nc);
          auto dx   = x_hc() - x_nc();

          RANGES_FOR (auto&& d, s.dimensions()) {
            auto gradients = compute_structured_gradient(s, bn, nc, d, lim);
            gradients *= dx(d);
            gradients += b_lhs(hc);
            b_lhs(hc) = gradients;
          }
        });
      }
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
  //           auto x_hc + b.centroid(hc);
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

template <typename State, typename TimeStepF, typename V>
num_t compute_time_step(State const& s, num_t cfl, TimeStepF&& t, V&& v) {
  num_t dt = std::numeric_limits<num_t>::max();
  for (auto&& b : s.blocks()) {
    auto&& lhs = s.time_integration.lhs(b);
    auto dx    = b.cell_length();
    b.for_each_internal(
     [&](auto c) { dt = std::min(dt, t(lhs(c), v, dx, cfl)); });
  }
  return dt;
}

template <typename State, typename TimeStepF, typename V>
num_t compute_time_step(State const& s, num_t cfl, TimeStepF&& t, V&& v,
                        num_t time, num_t time_end) {
  num_t dt = compute_time_step(s, cfl, t, v);
  if (time + dt > time_end) { dt = time_end - time; }
  return dt;
}

template <typename State> void cv_to_pv(State& s) {
  for (auto&& b : s.blocks()) {
    b.for_each([&](auto&& c) { s.physics.cv_to_pv(b.variables(c)); });
  }
}

template <typename State> void pv_to_cv(State& s) {
  for (auto&& b : s.blocks()) {
    b.for_each([&](auto&& c) { s.physics.pv_to_cv(b.variables(c)); });
  }
}

template <typename V, typename G> auto variables_at(V&& v, G&& g, num_t dx) {
  return v + g * dx;
}

template <typename NumFluxF, typename V, typename Block, typename CIdx,
          typename LHS>
num_a<std::decay_t<Block>::nvars()> structured_numerical_flux(
 NumFluxF&& nf, num_t dt, V&& v, Block&& b, CIdx&& c, LHS&& lhs) {
  using var_v  = num_a<std::decay_t<Block>::nvars()>;
  var_v result = var_v::Zero();
  struct {
    num_t dx, area, volume, dt;
  } data;
  data.dx         = b.cell_length();
  data.area       = b.cell_surface_area();
  data.volume     = b.cell_volume();
  data.dt         = dt;
  const num_t f   = 1. / data.dx;
  const num_t dx2 = data.dx / 2.;
  for (auto&& d : b.dimensions()) {
    auto cM = b.at(c, d, -1);
    auto cP = b.at(c, d, +1);

#define SECOND_ORDER
#ifdef SECOND_ORDER
    auto vM  = variables_at(lhs(cM), b.gradient(cM, d), +dx2);
    auto vcM = variables_at(lhs(c), b.gradient(c, d), -dx2);
    auto vcP = variables_at(lhs(c), b.gradient(c, d), +dx2);
    auto vP  = variables_at(lhs(cP), b.gradient(cP, d), -dx2);
#else
    auto vM  = lhs(cM);
    auto vcM = lhs(c);
    auto vcP = lhs(c);
    auto vP  = lhs(cP);
#endif
    result += (nf(v, vM, vcM, d, data) - nf(v, vcP, vP, d, data));
  }
  return f * result;
}

template <typename State, typename NumFluxF, typename VT>
void compute_structured_rhs(State&& s, NumFluxF&& nf, num_t dt, VT&& vt) {
  for (auto&& b : s.blocks()) {
    auto&& rhs = s.time_integration.rhs(b);
    b.for_each_internal([&](auto c) {
      rhs(c) += structured_numerical_flux(nf, dt, vt, b, c,
                                          s.time_integration.lhs(b));
    });
  }
}

// template <typename State> void advance_rhs(State& s) {
//   for (auto&& b : s.blocks()) {
//     b.for_each_internal([&](auto c) { b.variables(c) += b.rhs(c); });
//   }
// }

template <typename State, typename BCs, typename NumFlux, typename TimeStep,
          typename Limiter>
num_t advance_once(State& s, BCs& bcs, NumFlux&& nf, TimeStep&& ts,
                   Limiter&& li, uint_t time_step, num_t time, num_t time_end,
                   num_t cfl) {
  const auto cv = s.physics.cv();
  initialize_time_integration(s);

  bcs.apply(s, [&](auto&& b, auto&& c) -> auto {
    return s.time_integration.lhs(b)(c);
  });  // to variables

  const num_t dt = fv::compute_time_step(s, cfl, ts, cv, time, time_end);

  // Time integration loop:
  uint_t ti_step = 0;
  while (not s.time_integration.done()) {
    bcs.apply(s, [&](auto&& b, auto&& c) -> auto {
      return s.time_integration.lhs(b)(c);
    });
    {  // can happen in parallel
      fv::clear_rhs(s);
      // fv::clear_halos(s);  // in debug mode
    }
    fv::exchange_halos(s, li);

    std::cout << "step(it): " << time_step << "(" << ti_step
              << ") | time: " << time << " | dt: " << dt << std::endl;
    fv::compute_structured_gradients(s, li);
    // fv::correct_boundary_gradients(s, bcs);
    fv::compute_structured_rhs(s, nf, dt, cv);
    // fv::compute_boundary_rhs(s, nf, dt, cv);
    // fv::compute_source_term_rhs(s);
    s.time_integration.advance(s, dt);
    ti_step++;
    bcs.apply(s, [&](auto&& b, auto&& c) -> auto {
      return s.time_integration.lhs(b)(c);
    });
    fv::exchange_halos(s, li);
  }
  // vtk::serialize(s, "result", time_step, cv);
  // vtk::ls_serialize(s, bcs, "ls_result", time_step, cv);
  return dt;
}

template <typename State, typename BCs, typename NumFlux, typename TimeStep,
          typename Limiter>
uint_t advance_until(State& s, BCs& bcs, NumFlux&& nf, TimeStep&& ts,
                     Limiter&& lim, num_t time, num_t time_end,
                     uint_t time_step, num_t cfl) {
  while (not math::approx(time, time_end)) {
    num_t dt
     = advance_once(s, bcs, nf, ts, lim, time_step, time, time_end, cfl);
    time += dt;
    time_step++;
  }
  return time_step;
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
