#pragma once
/// \file
///
/// Utilities for creating views to signed-distance functions.
#include <hm3/geometry/primitive/point.hpp>

namespace hm3::geometry::sd {

namespace detail {

template <typename SDOperator, typename... SDFunctions>
struct view_state {
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

template <typename SDOperator, typename... SDFunctions>
auto view(SDOperator&& op, SDFunctions&... sdfs)
 -> detail::view_state<SDOperator, SDFunctions...> {
  return {std::forward<SDOperator>(op), std::forward<SDFunctions>(sdfs)...};
}

template <typename SDOperator, typename... SDFunctions>
auto view(SDOperator&& op, SDFunctions const&... sdfs)
 -> detail::view_state<SDOperator, SDFunctions...> {
  return {std::forward<SDOperator>(op), std::forward<SDFunctions>(sdfs)...};
}

}  // namespace hm3::geometry::sd
