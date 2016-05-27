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
  constexpr auto operator()(LeftVars&& vL, RightVars&& vR,
                            num_t const& distance) const noexcept {
    return (vR - vL) / distance;
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
  auto cm        = c.offset(d, -1);
  auto cp        = c.offset(d, +1);

  auto vM = lhs(cm);
  auto vP = lhs(cp);

  if (Same<Limiter, limiter::none_fn>{}) {  // unlimited
    static_assert(!std::is_const<Tile>{}, "");
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
