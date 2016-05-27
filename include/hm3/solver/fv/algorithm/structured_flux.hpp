#pragma once
/// \file
///
///

namespace hm3 {
namespace solver {
namespace fv {

template <typename NumFluxF, typename V, typename Tile, typename CIdx,
          typename LHS>
num_a<std::decay_t<V>::nvars()> structured_numerical_flux(NumFluxF&& nf,
                                                          num_t dt, V&& v,
                                                          Tile&& b, CIdx&& c,
                                                          LHS&& lhs) {
  using var_v       = num_a<std::decay_t<V>::nvars()>;
  auto variables_at = [](auto&& v, auto&& g, num_t dx) { return v + g * dx; };

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
    auto cM = c.offset(d, -1);
    auto cP = c.offset(d, +1);

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

}  // namespace fv
}  // namespace solver
}  // namespace hm3
