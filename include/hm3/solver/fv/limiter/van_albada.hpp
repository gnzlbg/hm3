#pragma once
/// \file
///
/// VanAlbada slope limiter
///
/// TODO: vanAlbada1 or vanAlbada2? Find paper and cite the right one
///
#include <hm3/solver/fv/state.hpp>

namespace hm3 {
namespace solver {
namespace fv {

namespace limiter {

struct van_albada_fn {
  template <typename Grad,
            typename vars = num_a<uncvref_t<Grad>::SizeAtCompileTime>>
  vars operator()(Grad&& g_l, Grad&& g_r) const noexcept {
    vars r = (g_l.array() / (g_r.array() + math::eps)).eval();
    vars rf
     = (r.array() * (1. + r.array()) / (1. + r.array() * r.array())).eval();
    vars rfr = (2. / (1. + r.array())).eval();
    return ((r.array() > 0.).template cast<num_t>()
            * rf.array().min(rfr.array()))
     .eval();
  }
};

namespace {
constexpr auto&& van_albada = static_const<van_albada_fn>::value;
}  // namespace

}  // namespace limiter
}  // namespace fv
}  // namespace solver
}  // namespace hm3
