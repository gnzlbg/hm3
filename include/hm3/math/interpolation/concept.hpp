#pragma once
/// \file
///
/// Interpolable concept: points + data
///
/// Some interpolation methods require e.g. that the points where the data is
/// sampled form a grid.
///
/// TODO: figure out a way to verify this at compile time? Or provide an
/// "StructuredInterpolable" concept?
#include <hm3/geometry/concept/ambient_dimension.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::math {

namespace ip {
namespace concepts {

/// Interpolable concept: points + data
struct interpolable : rc::refines<geometry::concept::AmbientDimension> {
  template <typename T>
  auto requires_(T&& t)
   -> decltype(rc::valid_expr(rc::model_of<rc::RandomAccessRange>(t.points()),
                              rc::model_of<rc::RandomAccessRange>(t.values())));
};

using Interpolable = interpolable;

}  // namespace concepts

template <typename T>
using Interpolable = rc::models<concepts::Interpolable, ranges::uncvref_t<T>>;

template <typename T, typename V>
using InterpolableV = meta::and_<
 rc::models<concepts::Interpolable, ranges::uncvref_t<T>>,
 Same<ranges::uncvref_t<decltype(*begin(std::declval<T>().values()))>,
      ranges::uncvref_t<T>>>;

}  // namespace ip
}  // namespace hm3::math
