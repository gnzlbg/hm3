#pragma once
/// \file
///
/// Normal
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace normal_detail {

struct normal_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(normal(std::forward<T>(t)));
};

}  // namespace normal_detail

namespace {
static constexpr auto const& normal
 = static_const<normal_detail::normal_fn>::value;
}  // namespace

}  // namespace hm3::geometry
