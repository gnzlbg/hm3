#pragma once
/// \file
///
/// Does one primitive contain another?
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace detail {

struct contains_fn {
  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(contains(std::forward<T>(t),
                                                 std::forward<U>(u)));
};

}  // namespace detail

namespace {
static constexpr auto const& contains
 = static_const<detail::contains_fn>::value;
}  // namespace

}  // namespace discrete

using discrete::contains;

}  // namespace geometry
}  // namespace hm3
