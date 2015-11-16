#pragma once
/// \file
///
/// Limiters
#include <hm3/solver/fv/state.hpp>
#include <hm3/utility/static_const.hpp>

namespace hm3 {
namespace solver {
namespace fv {

namespace limiter {

struct none_fn {
  template <typename Grad,
            typename vars = num_a<std::decay_t<Grad>::SizeAtCompileTime>>
  vars operator()(Grad&& /*g_l*/, Grad&& /*g_r*/) const noexcept {
    return vars::Ones();
  }
};

namespace {
constexpr auto&& none = static_const<none_fn>::value;
}  // namespace

struct minmod_fn {
  template <typename Grad,
            typename vars = num_a<std::decay_t<Grad>::SizeAtCompileTime>>
  vars operator()(Grad&& g_l, Grad&& g_r) const noexcept {
    return ((g_l.array() * g_r.array() > 0.).template cast<num_t>() *  //
            ((g_r.array().abs() > g_l.array().abs())
               .array()
               .template cast<num_t>()
              * g_l.array()
             +  //
             (g_l.array().abs() > g_r.array().abs())
               .array()
               .template cast<num_t>()
              * g_r.array()))
     .eval();
  }
};

namespace {
constexpr auto&& minmod = static_const<minmod_fn>::value;
}  // namespace

struct van_albada_fn {
  template <typename Grad,
            typename vars = num_a<std::decay_t<Grad>::SizeAtCompileTime>>
  vars operator()(Grad&& g_l, Grad&& g_r) const noexcept {
    vars r = (g_l.array() / (g_r.array() + math::eps<num_t>)).eval();
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
