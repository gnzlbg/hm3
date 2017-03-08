#pragma once
/// \file
///
/// Geometry forward declarations
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/geometry/algorithm/element_dimension.hpp>
#include <hm3/geometry/concept/associated_types.hpp>
#include <hm3/geometry/concept/integral_traits.hpp>
#include <hm3/geometry/concept/primitive_hierarchy.hpp>
#include <hm3/geometry/primitive/fwd.hpp>
#include <hm3/geometry/tolerance.hpp>
#include <hm3/types.hpp>

namespace hm3::geometry {

enum class non_degenerate_intersection_test_result {
  intersection,
  no_intersection,
  degenerate_intersection
};

}  // namespace hm3::geometry
