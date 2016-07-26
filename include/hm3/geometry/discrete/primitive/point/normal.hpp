#pragma once
/// \file
///
/// Normal vector of a 1D point.
#include <hm3/geometry/discrete/algorithm/normal.hpp>
#include <hm3/geometry/discrete/primitive/point/point.hpp>
#include <hm3/geometry/discrete/primitive/vector.hpp>

namespace hm3 {
namespace geometry {

namespace discrete {
namespace point_primitive {

/// Normal vector of a 1D point (positive in x direction)
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Nd == 1 and Same<UP, point<1>>{})>
constexpr vector<1> normal(P&&) noexcept {
  return vector<1>::constant(1.);
}

}  // namespace point_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
