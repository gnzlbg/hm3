#pragma once
/// \file
///
/// Dimension of an object
#include <hm3/utility/static_const.hpp>

namespace hm3 {
namespace geometry {

namespace adl_dimension_detail {
template <typename T>
static constexpr auto dimension(T&& t) noexcept -> decltype(t.dimension()) {
  return t.dimension();
}

struct dimension_fn {
  template <typename T>  // fall back to non-member function
  static constexpr auto impl(T&& t, long) noexcept -> decltype(dimension(t)) {
    return dimension(t);
  }
  template <typename T>  // prefer member function
  static constexpr auto impl(T&& t, int) noexcept -> decltype(t.dimension()) {
    return t.dimension();
  }

  template <typename T>  //
  constexpr auto operator()(T&& t) const noexcept {
    return this->impl(std::forward<T>(t), 0);
  }
};

namespace {
constexpr auto&& dimension = static_const<dimension_fn>::value;
}  // namespace

}  // namespace adl_dimension detail

using adl_dimension_detail::dimension;

template <typename T>
using dimension_t = ranges::uncvref_t<decltype(dimension(std::declval<T>()))>;

}  // namespace geometry

using geometry::dimension;

}  // namespace hm3
