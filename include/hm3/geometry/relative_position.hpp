#pragma once
/// \file
///
/// Relative position type.
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

/// Relative position between two geometric primitives (or one primitive and a
/// signed-distance function).
enum class relative_position_t : suint_t {
  inside      = 0,
  outside     = 1,
  intersected = 2
};

static constexpr relative_position_t inside  = relative_position_t::inside;
static constexpr relative_position_t outside = relative_position_t::outside;
static constexpr relative_position_t intersected
 = relative_position_t::intersected;

template <typename OStream>
OStream& to_ascii(OStream& os, relative_position_t p) {
  switch (p) {
    case inside: {
      os << "inside";
      break;
    }
    case outside: {
      os << "outside";
      break;
    }
    case intersected: {
      os << "intersected";
      break;
    }
  }
  return os;
}

/// Invert the relative position \p o.
relative_position_t invert(relative_position_t o) {
  switch (o) {
    case inside: {
      return outside;
    }
    case outside: {
      return inside;
    }
    case intersected: {
      return intersected;
    }
  }
}

struct is_inside_fn {
  constexpr bool operator()(relative_position_t i) const noexcept {
    return i == relative_position_t::inside;
  }
};

struct is_outside_fn {
  constexpr bool operator()(relative_position_t i) const noexcept {
    return i == relative_position_t::outside;
  }
};

struct is_intersected_fn {
  constexpr bool operator()(relative_position_t i) const noexcept {
    return i == relative_position_t::intersected;
  }
};

namespace {
static constexpr auto const& is_inside = static_const<is_inside_fn>::value;
}

namespace {
static constexpr auto const& is_outside = static_const<is_outside_fn>::value;
}

namespace {
static constexpr auto const& is_intersected
 = static_const<is_intersected_fn>::value;
}

}  // namespace hm3::geometry
