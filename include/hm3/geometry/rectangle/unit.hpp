#pragma once
/// \file
///
/// Unit
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/rectangle/concept.hpp>

namespace hm3 {
namespace geometry {

template <typename Shape, CONCEPT_REQUIRES_(Rectangle<Shape>{})>
auto unit(Shape) noexcept {
  static constexpr dim_t nd = dimension(Shape());
  return Shape{point<nd>::constant(0.), point<nd>::constant(1.)};
}

}  // namespace geometry
}  // namespacehm3
