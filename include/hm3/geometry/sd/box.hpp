#pragma once
/// \file
///
/// Signed-distance function for a box (inverse of a square)
#include <hm3/geometry/sd/combinators.hpp>
#include <hm3/geometry/sd/square.hpp>

namespace hm3 {
namespace geometry {
namespace sd {

template <int_t Nd> using fixed_box = inverted<fixed_square<Nd>>;

}  // namespace sd
}  // namespace geometry
}  // namespace hm3
