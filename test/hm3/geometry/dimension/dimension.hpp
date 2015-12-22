#pragma once
/// \file
///
/// Dimension testing utilities
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

struct with_member_dimension {
  static constexpr hm3::uint_t dimension() { return 2; }
  static auto dimensions() { return hm3::view::iota(0, 2); }
};

struct with_member_dimension2 {
  static constexpr hm3::uint_t dimension() { return 2; }
};

namespace other {

struct with_nonmember_dimension {};

constexpr hm3::uint_t dimension(with_nonmember_dimension const&) { return 3; }

auto dimensions(with_nonmember_dimension const&) {
  return hm3::view::iota(0, 3);
}

}  // namespace other

namespace other {

struct with_nonmember_dimension2 {};

constexpr hm3::uint_t dimension(with_nonmember_dimension2 const&) { return 3; }

}  // namespace other
