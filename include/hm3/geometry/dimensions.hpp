#pragma once
/// \file
///
/// Dimension range
#include <hm3/types.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/fatal_error.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {

/// Range of spatial dimensions: [0, nd)
///
/// TODO: make constexpr when view::iota is constexpr
[[ always_inline, flatten ]] inline auto dimensions(uint_t nd) noexcept {
  return view::iota(0_u, nd);
}
HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(dimensions(1));
HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(dimensions(2));
HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(dimensions(3));

/// Has a dimension
template <uint_t Nd> struct dimensional {
  static constexpr uint_t dimension() noexcept { return Nd; }
  static constexpr auto dimensions() noexcept {
    return ::hm3::geometry::dimensions(Nd);
  }
  HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(dimensions());
  CONCEPT_REQUIRES(Nd == 1)
  static string dimension_name(suint_t) { return "x"; }
  CONCEPT_REQUIRES(Nd == 2)
  static string dimension_name(suint_t d) { return d == 0 ? "x" : "y"; }
  CONCEPT_REQUIRES(Nd == 3)
  static string dimension_name(suint_t d) {
    HM3_ASSUME(d >= 0 and d < Nd);
    switch (d) {
      case 0: {
        return "x";
      }
      case 1: {
        return "y";
      }
      case 2: {
        return "z";
      }
      default: { HM3_FATAL_ERROR("¯\\_(ツ)_/¯"); }
    }
  }
};

namespace concepts {
namespace rc = ranges::concepts;

struct dimensioned {
  template <typename T>
  auto requires_(T&& t) -> decltype(
   rc::valid_expr(rc::convertible_to<uint_t>(t.dimension()),           //
                  rc::model_of<rc::RandomAccessRange>(t.dimensions())  //
                  ));
};

using Dimensioned = dimensioned;

}  // namespace concepts

template <typename T>
using Dimensioned
 = concepts::rc::models<concepts::Dimensioned, ranges::uncvref_t<T>>;

}  // namespace geometry

using geometry::dimensions;
using geometry::dimensional;

}  // namespace hm3
