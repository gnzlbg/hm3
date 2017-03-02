#pragma once
/// \file
///
/// Resizable concept.
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace concept {

struct Resizable {
  template <typename T, typename U = ranges::range_value_t<T>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(  //
   ((void)t.push_back(std::declval<U>()), 42),       //
   ((void)t.reserve(std::size_t{0}), 42)             //
   ));
};

}  // namespace concept

/// Resizable concept:
///
/// Requires:
/// - push_back
/// - reserve
template <typename T>
using Resizable = concept::rc::models<concept::Resizable, T>;

}  // namespace hm3::geometry
