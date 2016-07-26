#pragma once
/// \file
///
/// Normal
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace detail {

struct normal_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(normal(std::forward<T>(t)));
};

}  // namespace detail

namespace {
static constexpr auto const& normal = static_const<detail::normal_fn>::value;
}  // namespace

}  // namespace discrete

using discrete::normal;

}  // namespace geometry
}  // namespace hm3
