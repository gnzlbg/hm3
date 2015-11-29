#pragma once
/// \file
///
/// Simplices implementations
#include <array>
#include <hm3/types.hpp>
#include <hm3/geometry/dimensions.hpp>
#include <hm3/geometry/point.hpp>

namespace hm3 {
namespace geometry {

/// Simplex
template <uint_t Nd>  //
struct simplex : dimensional<Nd> {
  using point_t = geometry::point<Nd>;
  std::array<point_t, Nd> corners;
  point_t normal;
};

/// Heavy Simplex
template <uint_t Nd>  //
struct heavy_simplex : simplex<Nd> {
  geometry::point<Nd> center;
  num_t area;
};

}  // namespace geometry
}  // namespace hm3
