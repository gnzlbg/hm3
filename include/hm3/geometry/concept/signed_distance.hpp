#pragma once
/// \file
///
/// Signed distance function concept
#include <hm3/geometry/concept/fwd.hpp>
#include <hm3/geometry/concept/point.hpp>

namespace hm3::geometry {

namespace concept {

struct SignedDistance {
  template <typename T, typename U>
  auto requires_(T&& t, U&& u) -> decltype(rc::valid_expr(  //
   rc::convertible_to<num_t>(t(u))                          //
   ));
};

}  // namespace concept

template <typename T, typename P, dim_t Ad = ad_v<T>>
using SignedDistance
 = meta::and_<AmbientDimension<T, Ad>, Point<P, Ad>,
              concept::rc::models<concept::SignedDistance, T, P>>;

}  // namespace hm3::geometry
