#pragma once
/// \file
///
/// Signed-distance function for a box (inverse of a square)
#include <hm3/geometry/sd/adapt.hpp>
#include <hm3/geometry/sd/combinators.hpp>
#include <hm3/geometry/sd/square.hpp>

namespace hm3 {
namespace geometry {
namespace sd {

template <int_t Nd, typename... Args> auto fixed_box(Args&&... args) {
  return move_adapt(op_inverse,
                    fixed_rectangle<Nd>(std::forward<Args>(args)...));
}

}  // namespace sd
}  // namespace geometry
}  // namespace hm3
