#pragma once
/// \file
///
///
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/square.hpp>
#include <hm3/solver/fv/models/advection/indices.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace advection {

/// Initial conditions for the advection equation
namespace ic {

template <int Nd> struct square {
  using i        = indices<Nd>;
  using var_v    = num_a<i::nvars()>;
  using point_t  = geometry::point<Nd>;
  using square_t = geometry::square<Nd>;

  square_t square_;
  num_t inside_;
  num_t outside_;

  square(square_t s, num_t i, num_t o) : square_(s), inside_(i), outside_(o) {}

  num_a<1> operator()(point_t x) const noexcept {
    num_a<1> v;
    v(0) = geometry::contains(square_, x) ? inside_ : outside_;
    return v;
  }
};

}  // namespace ic

}  // namespace advection
}  // namespace fv
}  // namespace solver
}  // namespace hm3
