#pragma once
/// \file
///
/// Normal vector of a 1D point.
#include <hm3/geometry/algorithm/normal.hpp>
#include <hm3/geometry/primitive/point/point.hpp>
#include <hm3/geometry/primitive/vec.hpp>

namespace hm3::geometry::point_primitive {

/// Normal vector of a 1D point (positive in x direction)
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Nd == 1 and Same<UP, point<1>>{})>
constexpr vec<1> normal(P&&) noexcept {
  return vec<1>::constant(1.);
}

}  // namespace hm3::geometry::point_primitive
