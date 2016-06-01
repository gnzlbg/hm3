#pragma once
/// \file
///
/// Structured gradient
#include <hm3/solver/fv/algorithm/limit_gradient.hpp>
#include <hm3/solver/fv/limiter/none.hpp>
#include <hm3/types.hpp>

namespace hm3 {
namespace solver {
namespace fv {

struct structured_central_difference_fn {
  ///
  template <typename CIdx, typename VS>
  static constexpr auto unlimited(CIdx c, VS&& vs, dim_t d, num_t dx) {
    const auto dx2 = 2. * dx;
    auto c_m       = c.offset(d, -1);
    auto c_p       = c.offset(d, +1);

    auto v_m = vs(c_m);
    auto v_p = vs(c_p);
    return math::gradient(v_m, v_p, dx2).eval();
  }

  template <typename CIdx, typename VS, typename Limiter>
  static constexpr auto limited(CIdx c, VS&& vs, dim_t d, num_t dx,
                                Limiter&& limiter) {
    if (Same<uncvref_t<Limiter>, limiter::none_fn>{}) {
      return unlimited(c, std::forward<VS>(vs), d, dx);
    }
    auto c_m = c.offset(d, -1);
    auto c_p = c.offset(d, +1);

    auto v_m = vs(c_m);
    auto v_p = vs(c_p);
    auto v_c = vs(c);

    auto g_m = math::gradient(v_m, v_c, dx);
    auto g_p = math::gradient(v_c, v_p, dx);
    auto g_c = math::gradient(v_m, v_p, 2. * dx);

    return limit_gradient(limiter, g_m, g_c, g_p);
  };

  template <typename Tile, typename VS, typename Limiter = limiter::none_fn>
  constexpr void operator()(Tile& t, VS&& vs, Limiter limiter = Limiter{}) const
   noexcept {
    // compute and store the unlimited gradients first
    auto dx = t.geometry().cell_length();
    t.cells().for_each_internal(
     [&](auto c) {
       for (auto d : t.dimensions()) {
         t.gradient(c, d) = unlimited(c, std::forward<VS>(vs), d, dx);
       }
     },
     1 /* computes gradients until the first halo layer */);

    if (std::is_same<uncvref_t<Limiter>, limiter::none_fn>{}) { return; }

    // limit the gradients afterwards
    t.cells().for_each_internal(
     [&](auto c) {
       for (auto d : t.dimensions()) {
         auto c_m = c.offset(d, -1);
         auto c_p = c.offset(d, +1);
         t.gradient(c, d) = limit_gradient(
          limiter, t.gradient(c_m, d), t.gradient(c, d), t.gradient(c_p, d));
       }
     },
     1 /* computes gradients until the first halo layer */);
  }
};

namespace {
static constexpr auto&& structured_central_difference
 = static_const<structured_central_difference_fn>::value;
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
