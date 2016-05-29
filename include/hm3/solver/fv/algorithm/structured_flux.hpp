#pragma once
/// \file
///
///
#include <hm3/types.hpp>
#include <hm3/utility/matrix.hpp>
#include <type_traits>

namespace hm3 {
namespace solver {
namespace fv {

template <typename NumFluxF, typename V, typename Tile, typename CIdx,
          typename LHS>
num_a<std::decay_t<V>::nvars()> structured_numerical_flux(NumFluxF&& nf,
                                                          num_t dt, V&& v,
                                                          Tile&& b, CIdx&& c,
                                                          LHS&& lhs) {
  using var_v = num_a<std::decay_t<V>::nvars()>;
  auto variables_at
   = [](auto&& vidx, auto&& g, num_t dx) { return vidx + g * dx; };

  var_v result = var_v::Zero();
  struct {
    num_t dx, area, volume, dt;
  } data;
  data.dx         = b.geometry().cell_length();
  data.area       = b.geometry().cell_surface_area();
  data.volume     = b.geometry().cell_volume();
  data.dt         = dt;
  const num_t f   = 1. / data.dx;
  const num_t dx2 = data.dx / 2.;
  for (auto&& d : b.dimensions()) {
    auto c_m = c.offset(d, -1);
    auto c_p = c.offset(d, +1);

#define SECOND_ORDER
#ifdef SECOND_ORDER
    auto v_m  = variables_at(lhs(c_m), b.gradient(c_m, d), +dx2);
    auto v_cm = variables_at(lhs(c), b.gradient(c, d), -dx2);
    auto v_cp = variables_at(lhs(c), b.gradient(c, d), +dx2);
    auto v_p  = variables_at(lhs(c_p), b.gradient(c_p, d), -dx2);
#else
    auto v_m  = lhs(c_m);
    auto v_cm = lhs(c);
    auto v_cp = lhs(c);
    auto v_p  = lhs(c_p);
#endif
    result += (nf(v, v_m, v_cm, d, data) - nf(v, v_cp, v_p, d, data));
  }
  return f * result;
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
