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
#include <hm3/geometry/dimension.hpp>

namespace hm3 {
namespace ip {
namespace concepts {

namespace rc = ranges::concepts;

/// Interpolable concept: points + data
struct interpolable : rc::refines<geometry::concepts::Dimensional> {
  template <typename T>
  auto requires_(T&& t)
   -> decltype(rc::valid_expr(rc::model_of<rc::RandomAccessRange>(t.points()),
                              rc::model_of<rc::RandomAccessRange>(t.values())));
};

using Interpolable = interpolable;

}  // namespace concepts

template <typename T>
using Interpolable
 = concepts::rc::models<concepts::Interpolable, ranges::uncvref_t<T>>;

template <typename T, typename V>
using InterpolableV = meta::
 and_<concepts::rc::models<concepts::Interpolable, ranges::uncvref_t<T>>,
      Same<ranges::uncvref_t<decltype(*begin(std::declval<T>().values()))>,
           ranges::uncvref_t<T>>>;

}  // namespace ip
}  // namespace hm3
