#pragma once
/// \file
///
///
#include <hm3/geometry/point.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/config/assert.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace advection {

template <dim_t Nd> struct state {
  using point_t = geometry::point<Nd>;
  using var_v   = num_a<Nd>;
  /// Advection velocity
  num_a<Nd> velocity;

  state()             = default;
  state(state const&) = default;
  state(state&&)      = default;
  state& operator=(state const&) = default;
  state& operator=(state&&) = default;

  state(num_a<Nd> u) : velocity(u) {}
};

}  // namespace advection
}  // namespace fv
}  // namespace solver
}  // namespace hm3
