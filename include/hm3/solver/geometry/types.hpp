#pragma once
/// \file
///
/// Solver geometry
#include <hm3/geometry/all.hpp>

namespace hm3::solver {

/// Solver geometry.
namespace geometry {

/// Boundary Index
using bidx_t = optional_idx<suint_t, struct boundary_idx>;

/// Wrapper around the hm3::geometry namespace
namespace hg {
using namespace ::hm3::geometry;
}  // namespace hg

}  // namespace geometry

}  // namespace hm3::solver
