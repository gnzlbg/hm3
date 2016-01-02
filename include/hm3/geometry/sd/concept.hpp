#pragma once
/// \file
///
/// Signed distance function concept
#include <hm3/geometry/point.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {

namespace geometry {
namespace concepts {

namespace rc = ranges::concepts;

struct signed_distance {
  template <typename T, typename U>
  auto requires_(T&& t, U &&)
   -> decltype(rc::valid_expr(rc::convertible_to<num_t>(
    t(std::declval<point<std::decay_t<U>::value>>()))));
};

using SignedDistance = signed_distance;

}  // namespace concepts

template <typename T, uint_t Nd>
using SignedDistance
 = concepts::rc::models<concepts::SignedDistance, ranges::uncvref_t<T>,
                        std::integral_constant<uint_t, Nd>>;

}  // namespace geometry
}  // namespace hm3
