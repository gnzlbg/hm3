#pragma once
/// \file
///
/// Solver geometry
#include <hm3/geometry/all.hpp>  // TODO: be more fine grained

namespace hm3::solver {

/// Wrapper around the hm3::geometry namespace
namespace hg {
using namespace ::hm3::geometry;
}  // namespace hg

/// Solver geometry.
namespace geometry {

/// Index of boundary
using boundary_idx_t = optional_idx<sidx_t, struct boundary_idx_tag>;

constexpr boundary_idx_t operator"" _b(
 unsigned long long int i) {  // NOLINT(google-runtime-int)
  return boundary_idx_t{static_cast<sidx_t>(i)};
}

/// Boundary surface index
using surface_idx_t = hg::bvh::simplex_idx;

}  // namespace geometry

using geometry::boundary_idx_t;
using geometry::operator""_b;
using geometry::surface_idx_t;

}  // namespace hm3::solver
