#pragma once
/// \file
///
/// Utilities for creating views to signed-distance functions.
#include <hm3/geometry/sd/signed_distance.hpp>

namespace hm3::geometry::sd {

namespace detail {

template <dim_t Ad, typename SDOperator, typename... SDFunctions>
struct view_state : signed_distance<Ad> {
  SDOperator op;
  std::tuple<SDFunctions...> sdfs;

  template <typename SDOp, typename... SDFs>
  view_state(SDOp&& o, SDFs&&... fs)
   : op(std::forward<SDOp>(o)), sdfs(std::forward<SDFs>(fs)...) {}

  template <typename Point>
  auto operator()(Point&& x) const {
    auto f = [&](auto&&... args) {
      return op(std::forward<Point>(x), std::forward<decltype(args)>(args)...);
    };
    return ranges::tuple_apply(f, sdfs);
  }
};

}  // namespace detail

template <typename SDOperator, typename SDFunction, typename... SDFunctions>
auto view(SDOperator&& op, SDFunction& sdf, SDFunctions&... sdfs)
 -> detail::view_state<ad_v<SDFunction>, SDOperator, SDFunction,
                       SDFunctions...> {
  return {std::forward<SDOperator>(op), std::forward<SDFunction>(sdf),
          std::forward<SDFunctions>(sdfs)...};
}

template <typename SDOperator, typename SDFunction, typename... SDFunctions>
auto view(SDOperator&& op, SDFunction const& sdf, SDFunctions const&... sdfs)
 -> detail::view_state<ad_v<SDFunction>, SDOperator, SDFunction,
                       SDFunctions...> {
  return {std::forward<SDOperator>(op), std::forward<SDFunction>(sdf),
          std::forward<SDFunctions>(sdfs)...};
}

}  // namespace hm3::geometry::sd
