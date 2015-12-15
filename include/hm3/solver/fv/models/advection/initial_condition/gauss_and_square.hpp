#pragma once
/// \file
///
///
#include <hm3/geometry/point.hpp>
#include <hm3/solver/fv/advection/indices.hpp>
#include <hm3/utility/constants.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace advection {

/// Initial conditions for the advection equation
namespace ic {

template <int Nd> struct constant {
  using i     = indices<Nd>;
  using var_v = num_a<i::nvars()>;

  var_v value;

  constant(var_v v) : value(v) {}

  var_v operator()(point_t) const noexcept { return value; }
}

template <int Nd>
struct square {
  using i        = indices<Nd>;
  using p        = pv_base<Nd>;
  using var_v    = num_a<i::nvars()>;
  using point_t  = geometry::point<Nd>;
  using square_t = geometry::square<Nd>;

  square_t square;
  var_v inside;
  var_v outside;

  square(square_t s, var_v i, var_v o) : square(s), inside(i), outside(o) {}

  var_v operator()(point_t) const noexcept {
    return geometry::contains(square, x) ? inside : outside;
  }
};

}  // namespace ic

}  // namespace advection
}  // namespace fv
}  // namespace solver
}  // namespace hm3