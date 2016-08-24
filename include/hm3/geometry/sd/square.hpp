#pragma once
/// \file
///
/// Signed-distance function for a square
#include <hm3/geometry/sd/rectangle.hpp>

namespace hm3 {
namespace geometry {
namespace sd {

template <int_t Nd>
struct fixed_square : fixed_rectangle<Nd> {
  fixed_square(point<Nd> x_c, num_t l) noexcept
   : fixed_rectangle<Nd>(x_c, vec<Nd>::constant(l)) {}
};

}  // namespace sd
}  // namespace geometry
}  // namespace hm3
