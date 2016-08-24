#pragma once
/// \file
///
/// At which side of the primitive does another primitive lie?
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace side_detail {

enum class side_t : char { pos, neg, neither };

template <typename OStream>
OStream& operator<<(OStream& os, side_t const& s) {
  switch (s) {
    case side_t::pos: {
      os << "+";
      break;
    }
    case side_t::neg: {
      os << "-";
      break;
    }
    case side_t::neither: {
      os << "neither";
      break;
    }
  }
  return os;
}

struct side_fn {
  template <typename T, typename U>
  static constexpr auto side_impl(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(side(std::forward<T>(t),
                                             std::forward<U>(u)));

  template <typename T, typename U>
  static constexpr auto side_impl(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(side(std::forward<U>(u),
                                             std::forward<T>(t)));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(side_impl(std::forward<T>(t),
                                                  std::forward<U>(u), 0));
};

}  // namespace side_detail

using side_detail::side_t;

namespace {
static constexpr auto const& side = static_const<side_detail::side_fn>::value;
}  // namespace

}  // namespace hm3::geometry
