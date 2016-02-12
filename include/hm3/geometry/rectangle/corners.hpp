#pragma once
/// \file
///
/// Rectangle/square corners
#include <array>
#include <hm3/geometry/dimension/dimension.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/rectangle/concept.hpp>
#include <hm3/geometry/rectangle/square.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/fatal_error.hpp>

namespace hm3 {
namespace geometry {

namespace rectangle_detail {

/// Rectangle corners relative position (+-1)
///
/// Order: Counter Clow Wise (CCW):
///
///       7         6
///        *-------*
///       /|      /|
///      / |     / |       x(1)   x(2)
///   3 *-------* 2|        ^   ^
///     |  *----|--*        |  /
///     | / 4   | / 5       | /
///     |/      |/          |/
///     *-------*           *------> x(0)
///     0       1
///
constexpr sint_t relative_corner_position(sidx_t corner_pos,
                                          sidx_t dimension) noexcept {
  switch (dimension) {
    case 0: {
      return (corner_pos + 1) / 2 % 2 ? 1 : -1;
    }
    case 1: {
      return (corner_pos / 2) % 2 ? 1 : -1;
    }
    case 2: {
      return (corner_pos / 4) % 4 ? +1 : -1;
    }
    default: { HM3_FATAL_ERROR("unknown dimension {}", dimension); }
  }
}

template <uint_t Nd, uint_t... Is>
constexpr point<Nd> relative_corner_position(
 suint_t corner_pos, std::integer_sequence<uint_t, Is...>) noexcept {
  return point<Nd>{
   static_cast<num_t>(relative_corner_position(corner_pos, Is))...};
}

template <uint_t Nd>
constexpr point<Nd> relative_corner_position(suint_t corner_pos) noexcept {
  return relative_corner_position<Nd>(corner_pos,
                                      std::make_integer_sequence<uint_t, Nd>{});
}

}  // namespace rectangle_detail

/// Rectangle corner positions
template <typename Shape, CONCEPT_REQUIRES_(Rectangle<Shape>{})>
constexpr auto corner_positions(Shape const&) noexcept {
  return view::iota(suint_t{0}, Shape::no_corners());
}

/// Corners
template <typename Shape, CONCEPT_REQUIRES_(Rectangle<Shape>{})>
constexpr auto corners(Shape const& s) noexcept {
  std::array<point<dimension(Shape())>, Shape::no_corners()> corners;
  auto ls                 = lengths(s);
  const auto half_lengths = 0.5 * ls();
  const auto x_c          = centroid(s);
  RANGES_FOR (auto&& c, corner_positions(s)) {
    const auto x_p
     = rectangle_detail::relative_corner_position<dimension(Shape())>(c);
    corners[c] = x_c().array() + half_lengths.array() * x_p().array();
  }
  return corners;
}

template <uint_t Nd>  // optimization for squares
constexpr auto corners(square<Nd> const& s) noexcept {
  std::array<point<Nd>, square<Nd>::no_corners()> corners;
  const auto l           = length(s);
  const auto half_length = 0.5 * l;
  const auto x_c         = centroid(s);
  RANGES_FOR (auto&& c, corner_positions(s)) {
    const auto x_p = rectangle_detail::relative_corner_position<Nd>(c);
    corners[c]     = x_c().array() + half_length * x_p().array();
  }
  return corners;
}

}  // namespace geometry
}  // namespace hm3
