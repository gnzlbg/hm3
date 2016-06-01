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

// struct three_point_fn {
//   template <typename V, typename VT, typename State,
//             typename Vars = num_a<std::decay_t<VT>::nvars()>>
//   constexpr Vars operator()(VT&& vt, V&& v_l, V&& v_r, dim_t, State&& s)
//   const
//    noexcept {
//     Vars f;
//     f(0) = (vt.temperature(v_l) - vt.temperature(v_r)) / s.dx;
//     return f;
//   }
// };

struct three_point_fn {
  static constexpr bool requires_cell_gradients = false;

  struct surface {
    static constexpr vidx_t nvars() { return 1; }
    template <typename VT, typename CV, typename D>
    surface(VT&& vt, CV&& cv_l, CV&& cv_r, D&& data)
     : normal_temperature_gradient{(vt.temperature(cv_r) - vt.temperature(cv_l))
                                   / data.dx} {}
    num_t normal_temperature_gradient;
    num_t thermal_diffusivity = math::pi;
  };

  template <typename Surface, typename S = std::decay_t<Surface>,
            typename Vars = num_a<S::nvars()>>
  constexpr Vars operator()(Surface&& s) const noexcept {
    Vars f;
    f(0) = s.thermal_diffusivity * s.normal_temperature_gradient;
    return f;
  }
};

namespace {
constexpr auto&& three_point = static_const<three_point_fn>::value;
}  // namespace

template <typename T> struct dump;

struct five_point_fn {
  static constexpr bool requires_cell_gradients = true;

  struct surface {
    static constexpr vidx_t nvars() { return 1; }
    template <typename VT, typename CV, typename Tile, typename XS, typename D>
    surface(VT&& vt, CV&& cv_l, CV&& cv_r, Tile&& t, XS x_s, D&& data) {
      auto r = (0.5 * (t.gradient(x_s.adjacent_neg(), x_s.d)
                       + t.gradient(x_s.adjacent_pos(), x_s.d)))
                .eval();
      normal_temperature_gradient = r(0);
    }
    num_t normal_temperature_gradient;
    num_t thermal_diffusivity = math::pi;
  };

  template <typename Surface, typename S = std::decay_t<Surface>,
            typename Vars = num_a<S::nvars()>>
  constexpr Vars operator()(Surface&& s) const noexcept {
    Vars f;
    f(0) = s.thermal_diffusivity * s.normal_temperature_gradient;
    return f;
  }
};

namespace {
constexpr auto&& five_point = static_const<five_point_fn>::value;
}  // namespace

}  // namespace flux
}  // namespace heat
}  // namespace fv
}  // namespace solver
}  // namespace hm3
