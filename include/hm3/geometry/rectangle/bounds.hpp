#pragma once
/// \file
///
/// Rectangle bounds
#include <hm3/geometry/point.hpp>

namespace hm3 {
namespace geometry {

template <uint_t Nd> struct rectangle_bounds { point<Nd> min, max; };

}  // namespace geometry

}  // namespace hm3
