#pragma once
/// \file
///
/// Square volume
#include <hm3/geometry/rectangle/concept.hpp>
namespace hm3 {
namespace geometry {

/// Volume
template <typename Shape, CONCEPT_REQUIRES_(Rectangle<Shape>{})>
constexpr auto volume(Shape const& s) noexcept {
  num_t vol    = 1;
  const auto l = lengths(s);
  RANGES_FOR (auto&& d, dimensions(s)) { vol *= l(d); }
  return vol;
}

}  // namespace geometry
}  // namespace hm3
