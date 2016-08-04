#pragma once
/// \file
///
/// Centroid
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace centroid_detail {

struct centroid_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(centroid(std::forward<T>(t)));
};

}  // namespace centroid_detail

namespace {
static constexpr auto const& centroid
 = static_const<centroid_detail::centroid_fn>::value;
}  // namespace

}  // namespace hm3::geometry
