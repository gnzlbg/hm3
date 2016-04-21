#pragma once
/// \file
///
/// Square volume
#include <hm3/geometry/rectangle/concept.hpp>
#include <hm3/geometry/rectangle/square.hpp>
namespace hm3 {
namespace geometry {

/// Volume of a rectangle
template <typename Shape, CONCEPT_REQUIRES_(Rectangle<Shape>{})>
constexpr auto volume(Shape const& s) noexcept {
  num_t vol    = 1;
  const auto l = lengths(s);
  RANGES_FOR (auto&& d, dimensions(s)) { vol *= l(d); }
  return vol;
}

template <dim_t Nd>  // optimization for squares
constexpr auto volume(square<Nd> const& s) noexcept {
  auto vol     = length(s);
  const auto l = vol;
  for (dim_t d = 1; d < Nd; ++d) { vol *= l; }
  return vol;
}

}  // namespace geometry
}  // namespace hm3
