#pragma once
/// \file
///
/// Interprets cell variables as conservative variables
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/solver/fv/models/heat/indices.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3::solver::fv::heat {

template <dim_t Ad>
struct physics : geometry::with_ambient_dimension<Ad>, indices<Ad> {
  struct tile_variables {
    template <typename Grid, typename Order = dense::col_major_t>
    using invoke = meta::list<  //
                                //  nothing for now
     >;
  };

  num_t thermal_diffusivity;

  using self = physics<Ad>;
  using i    = indices<Ad>;

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) temperature(V&& v) noexcept {
    return v(i::temperature(), 0);
  }

  self cv() const noexcept { return *this; }
  self pv() const noexcept { return *this; }

  physics(num_t thermal_diffusivity_)
   : thermal_diffusivity(thermal_diffusivity_) {}

  template <typename S, typename V, typename CellData>
  static void load(V&& v, S const& s, CellData&& cell_data) noexcept {
    cell_data.load("temperature", [&](auto c, auto&&) {
      auto t = s.variables(c);
      return v.temperature(t);
    });
  }
};

}  // namespace hm3::solver::fv::heat
