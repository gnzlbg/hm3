#pragma once
/// \file
///
/// Default Finite Volume Method
#include <hm3/solver/fv/algorithm/clear_cells.hpp>
#include <hm3/solver/fv/algorithm/compute_time_step.hpp>
#include <hm3/solver/fv/algorithm/set_halos.hpp>
#include <hm3/solver/fv/algorithm/structured_gradient.hpp>
#include <hm3/solver/fv/algorithm/structured_rhs.hpp>
#include <hm3/solver/fv/tile/cell_gradients.hpp>
#include <hm3/solver/fv/time_integration/initialization.hpp>

namespace hm3 {
namespace solver {
namespace fv {

struct grad_method {
  struct tile_variables {
    template <typename Grid, typename Physics, typename TimeInt,
              typename Order = dense::col_major_t>
    using invoke             = meta::list<   //
     cell_gradients<Grid, Physics::nvars()>  //
     >;
  };

  template <typename State, typename BCs, typename NumFlux, typename TimeStep,
            typename Limiter>
  static num_t advance_once(State& s, BCs& bcs, NumFlux&& nf, TimeStep&& ts,
                            Limiter&& li, uint_t time_step, num_t time,
                            num_t time_end, num_t cfl) {
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
      fv::set_halos(s, li);

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
      fv::set_halos(s, li);
    }
    // vtk::serialize(s, "result", time_step, cv);
    // vtk::ls_serialize(s, bcs, "ls_result", time_step, cv);
    return dt;
  }
};

using default_method = grad_method;

}  // namespace fv
}  // namespace solver
}  // namespace hm3
