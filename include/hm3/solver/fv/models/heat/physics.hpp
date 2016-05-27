#pragma once
/// \file
///
/// Interprets cell variables as conservative variables
#include <hm3/geometry/dimensions.hpp>
#include <hm3/solver/fv/models/heat/indices.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace heat {

template <dim_t Nd> struct physics : geometry::dimensional<Nd>, indices<Nd> {
  struct tile_variables {
    template <typename Grid, typename Order = dense::col_major_t>
    using invoke = meta::list<  //
     //  nothing for now
     >;
  };

  using self = physics<Nd>;
  using i    = indices<Nd>;

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) temperature(V&& v) noexcept {
    return v(i::temperature(), 0);
  }

  self cv() const noexcept { return *this; }
  self pv() const noexcept { return *this; }

  physics()               = default;
  physics(physics const&) = default;
  physics(physics&&)      = default;
  physics& operator=(physics const&) = default;
  physics& operator=(physics&&) = default;

  template <typename S, typename V, typename CellData>
  static void load(V&& v, S const& s, CellData&& cell_data) noexcept {
    cell_data.load("temperature", [&](auto c, auto&&) {
      auto t = s.variables(c);
      return v.temperature(t);
    });
  }
};

}  // namespace heat
}  // namespace fv
}  // namespace solver
}  // namespace hm3
