#pragma once
/// \file
///
/// Dimensonal concept (objects that have a spatial dimension)
#include <hm3/geometry/dimension/dimension.hpp>
#include <hm3/geometry/dimension/dimensions.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {

namespace concepts {

namespace rc = ranges::concepts;

struct dimensional {
  template <typename T>
  auto requires_(T&& t) -> decltype(
   rc::valid_expr(rc::convertible_to<uint_t>(dimension(t)),           //
                  rc::model_of<rc::RandomAccessRange>(dimensions(t))  //
                  ));
};

using Dimensional = dimensional;

}  // namespace concepts

template <typename T>
using Dimensional
 = concepts::rc::models<concepts::Dimensional, ranges::uncvref_t<T>>;

template <typename T, uint_t Nd>
using NDimensional
 = meta::and_<Dimensional<T>,
              meta::bool_<decltype(dimension(ranges::uncvref_t<T>())){} == Nd>>;

}  // namespace geometry
}  // namespace hm3
