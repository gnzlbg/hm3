#pragma once
/// \file
///
/// Relative position type.

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

}  // namespace hm3::geometry
