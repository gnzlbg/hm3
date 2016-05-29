#pragma once
/// \file
///
/// Square and rectangle concepts
///
/// Rectangles have bounds (can be spanned from a (x_min, x_max) pair).
///
/// Squares have a constant side length. This cannot be guaranteed at
/// compile-time for general shapes but only for some particular types.
/// The `is_square` trait is opt-in.
#include <hm3/geometry/dimension/concept.hpp>
#include <hm3/geometry/polygon/types.hpp>

namespace hm3 {
namespace geometry {

namespace concepts {
namespace rc = ranges::concepts;

struct rectangle : rc::refines<rc::Regular, Dimensional> {
  template <typename T>
  auto requires_(T&& t)
   -> decltype(rc::valid_expr(bounds(t), length(t, 0_u), x_min(t), x_max(t),
                              rc::uncvref_t<T>::no_corners()));
};

using Rectangle = rectangle;

template <typename T> struct is_square : std::false_type {};

struct square : rc::refines<Rectangle> {
  template <typename T>
  auto requires_(T&& t) -> decltype(rc::is_true(is_square<rc::uncvref_t<T>>{}),
                                    rc::valid_expr(length(t)));
};

using Square = square;

}  // namespace concepts

template <typename T>
using Rectangle
 = concepts::rc::models<concepts::Rectangle, ranges::uncvref_t<T>>;

template <typename T>
using Square = concepts::rc::models<concepts::Square, ranges::uncvref_t<T>>;

}  // namespace geometry
}  // namespace hm3
