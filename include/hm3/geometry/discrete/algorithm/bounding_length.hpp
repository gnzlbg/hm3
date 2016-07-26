#pragma once
/// \file
///
/// Bounding length.
#include <hm3/utility/range.hpp>
#include <hm3/types.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace detail {

struct bounding_length_fn {
  /// Max bounding length of \p t.
  template <typename T>
  static constexpr auto max(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   max_bounding_length(std::forward<T>(t)));

  /// All bounding lengths of \p t.
  template <typename T>
  static constexpr auto all(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   all_bounding_lengths(std::forward<T>(t)));

  /// Bounding length across dimension \p d of \p t.
  template <typename T>
  constexpr auto operator()(T&& t, dim_t d) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(bounding_length(std::forward<T>(t), d));
};

}  // namespace detail

namespace {
static constexpr auto const& bounding_length
 = static_const<detail::bounding_length_fn>::value;
}

}  // namespace discrete

using discrete::bounding_length;

}  // namespace geometry
}  // namespace hm3
