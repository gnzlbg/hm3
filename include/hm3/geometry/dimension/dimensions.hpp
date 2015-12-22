#pragma once
/// \file
///
/// Range [0, dimension) for a type
#include <hm3/geometry/dimension/dimension.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {

namespace geometry {

namespace dimensions_detail {

template <typename T, CONCEPT_REQUIRES_(
                       std::is_convertible<ranges::uncvref_t<T>, uint_t>{})>
constexpr auto dimensions(T&& t) {
  return view::iota(0_u, std::forward<T>(t));
}

template <typename T, CONCEPT_REQUIRES_(
                       !std::is_convertible<ranges::uncvref_t<T>, uint_t>{})>
constexpr auto dimensions(T&& t) {
  return dimensions(dimension(t));
}
}  // namespace detail

struct dimensions_fn {
  template <typename T>  // using geometry::dimensions
  static constexpr auto impl(T&& t, long) noexcept {
    using dimensions_detail::dimensions;
    return dimensions(std::forward<T>(t));
  }

  template <typename T>  // member function
  static constexpr auto impl(T&& t, int) noexcept -> decltype(t.dimensions()) {
    return t.dimensions();
  }

  template <typename T>  //
  constexpr auto operator()(T&& t) const noexcept {
    return this->impl(std::forward<T>(t), 0);
  }
};

namespace {
constexpr auto&& dimensions = static_const<dimensions_fn>::value;
}  // namespace

}  // namespace geometry

using geometry::dimensions;

/// Range of spatial dimensions: [0, \p nd)
///
/// TODO: make constexpr when view::iota is constexpr
HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(dimensions(1_u));
HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(dimensions(2_u));
HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(dimensions(3_u));

}  // namespace hm3
