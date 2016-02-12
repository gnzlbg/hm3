#pragma once
/// \file
///
/// Utilities for adapting signed distance functions
#include <hm3/utility/range.hpp>
#include <type_traits>

namespace hm3 {
namespace geometry {
namespace sd {

namespace detail {

template <typename T>
using rref_to_value
 = meta::if_<std::is_rvalue_reference<T>, std::remove_reference<T>, T>;

template <typename... Args> std::tuple<Args...> as_tuple(meta::list<Args...>);

template <typename List>
using rrefs_to_values = meta::transform<List, meta::quote<rref_to_value>>;

template <typename SDOperator, typename... SDFunctions> struct adaptor {
  using tuple_t
   = decltype(as_tuple(rrefs_to_values<meta::list<SDFunctions...>>{}));
  SDOperator op;
  tuple_t sdfs;

  template <typename SDOp, typename... SDFs>
  adaptor(SDOp&& o, SDFs&&... fs)
   : op(std::forward<SDOp>(o)), sdfs(std::forward<SDFs>(fs)...) {}

  template <typename Point> auto operator()(Point&& x) const {
    auto f = [&](auto&&... args) {
      return op(std::forward<Point>(x), std::forward<decltype(args)>(args)...);
    };
    return ranges::tuple_apply(f, sdfs);
  }
};

}  // namespace detail

template <typename SDOperator, typename... SDFunctions>
auto adapt(SDOperator&& op, SDFunctions&&... sdfs)
 -> detail::adaptor<SDOperator, SDFunctions...> {
  return {std::forward<SDOperator>(op), std::forward<SDFunctions>(sdfs)...};
}

}  // namespace sd
}  // namespace geometry
}  // namespace hm3
