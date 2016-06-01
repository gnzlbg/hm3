#pragma once
/// \file
///
/// AABB Concept
///
#include <hm3/geometry/dimension/concept.hpp>

namespace hm3 {
namespace geometry {

namespace concepts {
namespace rc = ranges::concepts;

struct aabb : rc::refines<rc::Regular, Dimensional> {
  template <typename T>
  auto requires_(T&& t)
   -> decltype(rc::valid_expr(x_max(t), x_min(t), bounds(t),
                              length(t, dim_t{0}), lengths(t)));
};

using AABB = aabb;

}  // namespace concepts

template <typename T>
using AABB = concepts::rc::models<concepts::AABB, ranges::uncvref_t<T>>;

}  // namespace geometry
}  // namespace hm3
