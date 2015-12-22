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
  template <typename T, uint_t Nd>
  auto requires_(T&& t, std::integral_constant<uint_t, Nd>)
   -> decltype(rc::valid_expr(num_t(t(point<Nd>()))));
};

using SignedDistance = signed_distance;

}  // namespace concepts

template <typename T, uint_t Nd>
using SignedDistance
 = concepts::rc::models<concepts::SignedDistance, ranges::uncvref_t<T>,
                        std::integral_constant<uint_t, Nd>>;

}  // namespace geometry
}  // namespace hm3
