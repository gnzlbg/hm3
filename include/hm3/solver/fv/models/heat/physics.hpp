#pragma once
/// \file
///
/// Interprets cell variables as conservative variables
#include <hm3/geometry/dimensions.hpp>
#include <hm3/solver/fv/heat/indices.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace heat {

template <uint_t Nd> struct physics : dimensional<Nd>, indices<Nd> {
  using self = physics<Nd>;

  template <typename V> static decltype(auto) temperature(V&& v) noexcept {
    return v(indices<Nd>::temperature());
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
