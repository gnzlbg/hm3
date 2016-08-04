#pragma once
/// \file
///
///
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/solver/fv/models/advection/indices.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace advection {

/// Initial conditions for the advection equation
namespace ic {

template <int Nd> struct constant {
  using i       = indices<Nd>;
  using var_v   = num_a<i::nvars()>;
  using point_t = geometry::point<Nd>;

  var_v value;

  constant(var_v v) : value(v) {}

  var_v operator()(point_t) const noexcept { return value; }
};

template <int Nd> struct box {
  using i       = indices<Nd>;
  using var_v   = num_a<i::nvars()>;
  using point_t = geometry::point<Nd>;
  using box_t   = geometry::box<Nd>;

  box_t box_;
  var_v inside;
  var_v outside;

  box(box_t s, var_v i, var_v o) : box_(s), inside(i), outside(o) {}

  var_v operator()(point_t x) const noexcept {
    return geometry::contains(box_, x) ? inside : outside;
  }
};

}  // namespace ic

}  // namespace advection
}  // namespace fv
}  // namespace solver
}  // namespace hm3
