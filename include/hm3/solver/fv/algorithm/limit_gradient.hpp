#ifdef ABCD
#pragma once
/// \file
///
/// Limits a gradient with a limiter
#include <hm3/solver/fv/limiter/none.hpp>
#include <hm3/types.hpp>

namespace hm3::solver::fv {

struct limit_gradient_fn {
  /// Limits the cell gradient \p cg_c with the \p limiter and the neighbor
  /// gradients \p cg_m and \p cg_p
  template <typename Limiter, typename CG>
  constexpr auto operator()(Limiter limiter, CG&& cg_m, CG&& cg_c,
                            CG&& cg_p) const noexcept {
    auto l = limiter(cg_m, cg_p);
    return (cg_c.array() * l.array()).matrix().eval();
  }
};

namespace {
constexpr auto const& limit_gradient = static_const<limit_gradient_fn>::value;
}  // namespace

}  // namespace hm3::solver::fv
#endif
