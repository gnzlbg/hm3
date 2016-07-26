#pragma once
/// \file
///
/// Closed polygon.
#include <hm3/geometry/discrete/primitive/polygon/bounding_length.hpp>
#include <hm3/geometry/discrete/primitive/polygon/bounding_volume.hpp>
#include <hm3/geometry/discrete/primitive/polygon/centroid.hpp>
#include <hm3/geometry/discrete/primitive/polygon/concept.hpp>
#include <hm3/geometry/discrete/primitive/polygon/degenerate.hpp>
#include <hm3/geometry/discrete/primitive/polygon/integral.hpp>
#include <hm3/geometry/discrete/primitive/polygon/normal.hpp>
//#include <hm3/geometry/discrete/primitive/polygon/intersection.hpp>
#include <hm3/geometry/discrete/primitive/polygon/bounded_polygon.hpp>
#include <hm3/geometry/discrete/primitive/polygon/fixed_polygon.hpp>
#include <hm3/geometry/discrete/primitive/polygon/ostream.hpp>
//#include <hm3/geometry/discrete/primitive/polygon/segments.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

/// Polygon primitive
namespace polygon_primitive {}

template <dim_t Nd> using triangle = fixed_polygon<Nd, 3>;
template <dim_t Nd> using quad     = fixed_polygon<Nd, 4>;

}  // namespace discrete

using discrete::fixed_polygon;
using discrete::bounded_polygon;
using discrete::triangle;
using discrete::quad;

}  // namespace geometry
}  // namespace hm
