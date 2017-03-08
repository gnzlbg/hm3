#pragma once
/// \file
///
///
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/assert.hpp>

namespace hm3::solver::fv::advection {

template <dim_t Ad>
struct state {
  using point_t = geometry::point<Ad>;
  using var_v   = num_a<Ad>;
  /// Advection velocity
  num_a<Ad> velocity;

  state()             = default;
  state(state const&) = default;
  state(state&&)      = default;
  state& operator=(state const&) = default;
  state& operator=(state&&) = default;

  state(num_a<Ad> u) : velocity(u) {}
};

}  // namespace hm3::solver::fv::advection
