#pragma once
/// \file
///
/// Distance computation.
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>
#include <hm3/geometry/discrete/algorithm/centroid.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace detail {

struct distance_fn {
  /// Distance from centroid to centroid
  template <typename T, typename U>
  static constexpr num_t centroid(T&& t, U&& u) noexcept {
    auto const& t_xc = discrete::centroid(std::forward<T>(t));
    auto const& u_xc = discrete::centroid(std::forward<U>(u));
    return (t_xc() - u_xc()).norm();
  }
};

}  // namespace detail

namespace {
static constexpr auto const& distance
 = static_const<detail::distance_fn>::value;
}

}  // namespace discrete

using discrete::distance;

}  // namespace geometry
}  // namespace hm3
