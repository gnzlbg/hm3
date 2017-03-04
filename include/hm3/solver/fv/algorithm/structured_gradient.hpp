#pragma once
/// \file
///
/// Structured gradient
#include <hm3/solver/fv/limiter/none.hpp>
#include <hm3/types.hpp>

namespace hm3 {
namespace solver {
namespace fv {

struct compute_structured_gradients_fn {
  /// If the gradients are not required, do nothing
  template <typename State, typename Limiter>
  static constexpr void impl(State&, Limiter, std::false_type) noexcept {}

  template <typename State, typename Limiter>
  static constexpr void impl(State& s, Limiter limiter,
                             std::true_type) noexcept {
    for (auto&& t : s.tiles()) {
      auto&& lhs = s.time_integration.lhs(t);
      structured_central_difference(t, lhs, limiter);
    }
  }

  template <typename State, typename Limiter = limiter::none_fn>
  constexpr void operator()(State& s, Limiter limiter = Limiter{}) const
   noexcept {
    using state_t    = uncvref_t<State>;
    using num_flux_t = typename state_t::numerical_flux_t;
    impl(s, limiter, meta::bool_<num_flux_t::requires_cell_gradients>{});
  }
};

namespace {
constexpr auto&& compute_structured_gradients
 = static_const<compute_structured_gradients_fn>::value;
}  // namespace
/*
template <typename State, typename Tile, typename CIdx, typename Limiter>
auto compute_local_central_difference_structured_gradient(State& s, Tile& b,
                                                          CIdx c, dim_t d,
                                                          Limiter& limiter) {
  using vars     = num_a<uncvref_t<State>::nvars()>;
  const auto dx  = b.geometry().cell_length();
  const auto dx2 = 2. * dx;
  auto&& lhs     = s.time_integration.lhs(b);
  auto c_m       = c.offset(d, -1);
  auto c_p       = c.offset(d, +1);

  auto v_m = lhs(c_m);
  auto v_p = lhs(c_p);

  if (Same<Limiter, limiter::none_fn>{}) {  // unlimited
    return vars(math::gradient(v_m, v_p, dx2));
  }

  // limited:
  auto v_c = lhs(c);

  // vars g_m  = math::gradient(v_c, v_m, dx);
  // vars g_p  = math::gradient(v_p, v_c, dx);
  // vars g_c  = math::gradient(v_p, v_m, dx2);

  vars g_m = math::gradient(v_m, v_c, dx);
  vars g_p = math::gradient(v_c, v_p, dx);
  vars g_c = math::gradient(v_m, v_p, dx2);

  vars lim = limiter(g_m, g_p);

  return vars(g_c.array() * lim.array());
}

struct compute_structured_gradients_fn {
  /// If the gradients are not required, do nothing
  template <typename State, typename Limiter>
  static constexpr void impl(State&, Limiter, std::false_type) noexcept {}

  template <typename State, typename Limiter>
  static constexpr void impl(State& s, Limiter limiter,
                             std::true_type) noexcept {
    for (auto&& b : s.tiles()) {
      b.cells().for_each_internal(
       [&](auto c) {
         for (auto d : ambient_dimensions(b)) {
           b.gradient(c, d)
            = compute_local_central_difference_structured_gradients(s, b, c, d,
                                                                    limiter);
         }
       },
       1 /* computes gradients until the first halo layer */ /*);
}
}

template <typename State, typename Limiter = limiter::none_fn>
constexpr void operator()(State& s, Limiter limiter = Limiter{}) const
noexcept {
using state_t    = uncvref_t<State>;
using num_flux_t = typename state_t::numerical_flux_t;
// if the numerical flux does not need the cell center gradients this does
// nothing:
impl(s, limiter, meta::bool_<num_flux_t::requires_cell_gradients>{});
}
};

namespace {
constexpr auto&& compute_structured_gradients
= static_const<compute_structured_gradients_fn>::value;
}  // namespace
*/
}  // namespace fv
}  // namespace solver
}  // namespace hm3
