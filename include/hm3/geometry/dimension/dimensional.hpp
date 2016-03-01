#pragma once
/// \file
///
/// Implements the Dimensional concept for a type + some goodies
#include <hm3/geometry/dimension/dimension.hpp>
#include <hm3/geometry/dimension/dimensions.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/config/fatal_error.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {

/// Implements the dimensional concept for a type
template <uint_t Nd>  //
struct dimensional {
  static constexpr std::integral_constant<uint_t, Nd> dimension() noexcept {
    return std::integral_constant<uint_t, Nd>{};
  }
  static constexpr auto dimensions() noexcept {
    return ::hm3::geometry::dimensions(Nd);
  }
  HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(dimensions());
  CONCEPT_REQUIRES(Nd == 1)
  static string dimension_name(suint_t) { return string(1, 'x'); }
  CONCEPT_REQUIRES(Nd == 2)
  static string dimension_name(suint_t d) {
    return string(1, d == 0 ? 'x' : 'y');
  }
  CONCEPT_REQUIRES(Nd == 3)
  static string dimension_name(suint_t d) {
    HM3_ASSUME(d >= 0 and d < Nd);
    char n[3] = {'x', 'y', 'z'};
    return string(1, n[d]);
  }
};

}  // namespace geometry
}  // namespace hm3
