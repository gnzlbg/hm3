#pragma once
/// \file
///
/// Structured gradient
#include <hm3/solver/fv/limiter/none.hpp>
#include <hm3/types.hpp>

namespace hm3 {
namespace solver {
namespace fv {

struct gradient_fn {
  template <typename LeftVars, typename RightVars>
  constexpr auto operator()(LeftVars&& v_l, RightVars&& v_r,
                            num_t const& distance) const noexcept {
    return (v_r - v_l) / distance;
  }
};

namespace {
constexpr auto&& gradient = static_const<gradient_fn>::value;
}  // namespace

template <typename State, typename Tile, typename CIdx, typename Limiter>
auto compute_structured_gradient(State& s, Tile& b, CIdx c, dim_t d,
                                 Limiter& limiter) {
  using vars = num_a<std::decay_t<State>::nvars()>;
  vars grad;
  const auto dx  = b.geometry().cell_length();
  const auto dx2 = 2. * dx;
  auto&& lhs     = s.time_integration.lhs(b);
  auto c_m       = c.offset(d, -1);
  auto c_p       = c.offset(d, +1);

  auto v_m = lhs(c_m);
  auto v_p = lhs(c_p);

  if (Same<Limiter, limiter::none_fn>{}) {  // unlimited
    static_assert(!std::is_const<Tile>{}, "");
    b.gradient(c, d) = gradient(v_m, v_p, dx2);
  } else {  // limited:
    auto v_c = lhs(c);

    // vars g_m  = gradient(v_c, v_m, dx);
    // vars g_p  = gradient(v_p, v_c, dx);
    // vars g_c  = gradient(v_p, v_m, dx2);

    vars g_m = gradient(v_m, v_c, dx);
    vars g_p = gradient(v_c, v_p, dx);
    vars g_c = gradient(v_m, v_p, dx2);

    vars lim = limiter(g_m, g_p);

    grad = g_c.array() * lim.array();
  }
  return grad;
}

template <typename State, typename Limiter = limiter::none_fn>
void compute_structured_gradients(State& s, Limiter limiter = Limiter{}) {
  for (auto&& b : s.tiles()) {
    b.cells().for_each_internal(
     [&](auto c) {
       for (auto d : b.dimensions()) {
         b.gradient(c, d) = compute_structured_gradient(s, b, c, d, limiter);
       }
     },
     1);
  }
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
