#pragma once
/// \file
///
#include <hm3/solver/fv/algorithm/structured_flux.hpp>

namespace hm3::solver::fv {

// template <typename State, typename NumFluxF, typename VT>
// void compute_structured_rhs(State&& s, NumFluxF&& nf, num_t dt, VT&& vt) {
//   for (auto&& b : s.tiles()) {
//     auto&& rhs = s.time_integration.rhs(b);
//     b.cells().for_each_internal([&](auto c) {
//       rhs(c) += structured_numerical_flux(nf, dt, vt, b, c,
//                                           s.time_integration.lhs(b));
//     });
//   }
// }

struct compute_structured_rhs_fn {
  template <typename State, typename VT>
  [[HM3_FLATTEN]] static constexpr void impl(State&& s, num_t dt, VT&& vt,
                                             std::false_type) noexcept {
    using surface_state = typename uncvref_t<State>::numerical_flux_t::surface;
    for (auto&& t : s.tiles()) {
      auto&& rhs = s.time_integration.rhs(t);
      auto&& lhs = s.time_integration.lhs(t);
      const struct {
        num_t dx, area, volume, dt;
      } data{t.geometry().cell_length(), t.geometry().cell_surface_area(),
             t.geometry().cell_volume(), dt};

      t.surfaces().for_each_internal([&](auto x_s) {
        auto cm  = x_s.adjacent_neg();
        auto cp  = x_s.adjacent_pos();
        auto cvm = lhs(cm);
        auto cvp = lhs(cp);
        auto sm  = surface_state(vt, cvm, cvp, data);
        auto f   = data.area * s.numerical_flux(sm);
        rhs(cm) += 1. / data.volume * f;
        rhs(cp) -= 1. / data.volume * f;
      });
    }
  }

  template <typename State, typename VT>
  [[HM3_FLATTEN]] static constexpr void impl(State&& s, num_t dt, VT&& vt,
                                             std::true_type) noexcept {
    using surface_state = typename uncvref_t<State>::numerical_flux_t::surface;
    for (auto&& t : s.tiles()) {
      auto&& rhs = s.time_integration.rhs(t);
      auto&& lhs = s.time_integration.lhs(t);
      const struct {
        num_t dx, area, volume, dt;
      } data{t.geometry().cell_length(), t.geometry().cell_surface_area(),
             t.geometry().cell_volume(), dt};

      t.surfaces().for_each_internal([&](auto x_s) {
        auto cm  = x_s.adjacent_neg();
        auto cp  = x_s.adjacent_pos();
        auto cvm = lhs(cm);
        auto cvp = lhs(cp);
        auto sm  = surface_state(vt, cvm, cvp, t, x_s, data);
        auto f   = data.area * s.numerical_flux(sm);
        rhs(cm) += 1. / data.volume * f;
        rhs(cp) -= 1. / data.volume * f;
      });
    }
  }

  template <typename State, typename VT>
  [[HM3_FLATTEN]] constexpr void operator()(State&& s, num_t dt, VT&& vt) const
   noexcept {
    using state_t    = uncvref_t<State>;
    using num_flux_t = typename state_t::numerical_flux_t;
    impl(s, dt, std::forward<VT>(vt),
         meta::bool_<num_flux_t::requires_cell_gradients>{});
  }
};

namespace {
constexpr auto const& compute_structured_rhs
 = static_const<compute_structured_rhs_fn>::value;
}

}  // namespace hm3::solver::fv
