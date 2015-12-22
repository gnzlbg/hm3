#pragma once
/// \file
///
/// Implicit geometries using signed-distance functions.
#include <hm3/geometry/sd/box.hpp>
#include <hm3/geometry/sd/combinators.hpp>
#include <hm3/geometry/sd/concept.hpp>
#include <hm3/geometry/sd/edge.hpp>
#include <hm3/geometry/sd/sphere.hpp>

namespace hm3 {
namespace geometry {

/// Implicit signed-distance functions
///
/// Outside:  g(x) > 0 outside,
/// Surface: g(x) = 0, and
/// Inside:  g(x) < 0 .
namespace sd {}  // namespace sd

}  // namespace geometry
}  // namespace hm3
