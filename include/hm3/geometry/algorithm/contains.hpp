#pragma once
/// \file
///
/// Does one primitive contain another?
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace contains_detail {

struct contains_fn {
  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(contains(std::forward<T>(t),
                                                 std::forward<U>(u)));
};

}  // namespace contains_detail

namespace {
static constexpr auto const& contains
 = static_const<contains_detail::contains_fn>::value;
}  // namespace

}  // namespace hm3::geometry
