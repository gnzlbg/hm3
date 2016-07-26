#pragma once
/// \file
///
/// Centroid
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace detail {

struct centroid_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(centroid(std::forward<T>(t)));
};

}  // namespace detail

namespace {
static constexpr auto const& centroid
 = static_const<detail::centroid_fn>::value;
}  // namespace

}  // namespace discrete

using discrete::centroid;

}  // namespace geometry
}  // namespace hm3
