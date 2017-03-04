#pragma once
/// \file
///
/// Interprets cell variables as conservative variables
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/solver/fv/models/advection/indices.hpp>
#include <hm3/solver/fv/models/advection/state.hpp>

namespace hm3::solver::fv::advection {

template <dim_t Ad>
struct physics : state<Ad>, geometry::with_ambient_dimension<Ad>, indices<Ad> {
  using self = physics<Ad>;

  template <typename V>
  static decltype(auto) marker(V&& v) noexcept {
    return v(indices<Ad>::marker());
  }

  template <typename V>
  decltype(auto) max_wave_speed(V&& /*v*/, dim_t d) const noexcept {
    return std::abs(this->velocity(d));
  }

  template <typename V>
  num_a<1> flux(V&& v, dim_t d) const noexcept {
    using vars = num_a<1>;
    vars f     = vars::Zero();
    f(0)       = this->velocity(d) * v(0);
    return f;
  }

  self cv() const noexcept { return *this; }
  self pv() const noexcept { return *this; }

  physics()               = default;
  physics(physics const&) = default;
  physics(physics&&)      = default;
  physics& operator=(physics const&) = default;
  physics& operator=(physics&&) = default;

  physics(num_a<Ad> u) : state<Ad>(u) {}

  template <typename S, typename V, typename CellData>
  static void load(V&& v, S const& s, CellData&& cell_data) noexcept {
    cell_data.load("marker", [&](auto c, auto&&) {
      auto t = s.variables(c);
      return v.marker(t);
    });
    cell_data.load(
     "velocity", [&](auto /*c*/, auto&& d) { return s.physics.velocity(d); },
     ambient_dimensions(v), [&](auto&& d) { return v.dimension_name(d); });
  }
};

}  // namespace hm3::solver::fv::advection
