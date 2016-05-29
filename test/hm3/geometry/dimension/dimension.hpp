#pragma once
/// \file
///
/// Dimension testing utilities
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

struct with_member_dimension {
  static constexpr auto dimension() {
    return std::integral_constant<hm3::dim_t, 2>{};
  }
  static auto dimensions() {
    return hm3::view::iota(hm3::dim_t{0}, hm3::dim_t{2});
  }
};

struct with_member_dimension2 {
  static constexpr auto dimension() {
    return std::integral_constant<hm3::dim_t, 2>{};
  }
};

namespace other {

struct with_nonmember_dimension {};

constexpr auto dimension(with_nonmember_dimension const&) {
  return std::integral_constant<hm3::dim_t, 3>{};
}

inline auto dimensions(with_nonmember_dimension const&) {
  return hm3::view::iota(hm3::dim_t{0}, hm3::dim_t{3});
}

}  // namespace other

namespace other {

struct with_nonmember_dimension2 {};

constexpr auto dimension(with_nonmember_dimension2 const&) {
  return std::integral_constant<hm3::dim_t, 3>{};
}

}  // namespace other
