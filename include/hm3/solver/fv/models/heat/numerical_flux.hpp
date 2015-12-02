#pragma once
/// \file
///
/// Numerical flux
#include <hm3/geometry/dimensions.hpp>
#include <hm3/solver/fv/heat/indices.hpp>
#include <hm3/utility/matrix.hpp>
#include <hm3/utility/static_const.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace heat {

namespace flux {

struct three_point_fn {
  template <typename V, typename VT, typename State,
            typename var_v = num_a<std::decay_t<VT>::nvars()>>
  constexpr var_v operator()(VT&& vt, V&& v_l, V&& v_r, suint_t,
                             State&& s) const noexcept {
    var_v f;
    f(0) = (vt.temperature(v_l) - vt.temperature(v_r)) / s.dx;
    return f;
  }
};

namespace {
constexpr auto&& three_point = static_const<three_point_fn>::value;
}  // namespace

}  // namespace flux
}  // namespace heat
}  // namespace fv
}  // namespace solver
}  // namespace hm3
