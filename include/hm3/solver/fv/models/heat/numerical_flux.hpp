#pragma once
/// \file
///
/// Numerical flux
#include <hm3/geometry/dimensions.hpp>
#include <hm3/solver/fv/models/heat/indices.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace heat {

namespace flux {

struct three_point_fn {
  template <typename V, typename VT, typename State,
            typename Vars = num_a<std::decay_t<VT>::nvars()>>
  constexpr Vars operator()(VT&& vt, V&& v_l, V&& v_r, dim_t, State&& s) const
   noexcept {
    Vars f;
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
